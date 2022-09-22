#include "node.h"

DEFINE_bool(raft_step_down_when_vote_timedout, true, 
            "candidate steps down when reaching timeout");
DEFINE_int32(raft_apply_batch, 64, "batch number of applying tasks");
BRPC_VALIDATE_GFLAG(raft_step_down_when_vote_timedout, brpc::PassValidate);

struct GlobalExtension {
    SegmentLogStorage local_log;
    LocalRaftMetaStorage local_meta;
};

DEFINE_int32(raft_election_heartbeat_factor, 10, "raft election:heartbeat timeout factor");
static inline int heartbeat_timeout(int election_timeout) {
    return std::max(election_timeout / FLAGS_raft_election_heartbeat_factor, 10);
}

static void global_init_or_die_impl() {
    static GlobalExtension s_ext;

    log_storage_extension()->RegisterOrDie("local", &s_ext.local_log);
    meta_storage_extension()->RegisterOrDie("local", &s_ext.local_meta);
}

class LeaderStableClosure : public LogManager::StableClosure {
public:
    void Run();
private:
    LeaderStableClosure(const NodeId& node_id,
                        size_t nentries,
                        BallotBox* ballot_box);
    ~LeaderStableClosure() {}
friend class NodeImpl;
    NodeId _node_id;
    size_t _nentries;
    BallotBox* _ballot_box;
};

LeaderStableClosure::LeaderStableClosure(const NodeId& node_id,
                                         size_t nentries,
                                         BallotBox* ballot_box)
    : _node_id(node_id), _nentries(nentries), _ballot_box(ballot_box)
{
}

void LeaderStableClosure::Run() {
    if (status().ok()) {
        if (_ballot_box) {
            // ballot_box check quorum ok, will call fsm_caller
            _ballot_box->commit_at(
                    _first_log_index, _first_log_index + _nentries - 1, _node_id.peer_id);
        }
    } else {
        LOG(ERROR) << "node " << _node_id << " append [" << _first_log_index << ", "
                   << _first_log_index + _nentries - 1 << "] failed";
    }
    delete this;
}



static void print_revision(std::ostream& os, void*) {
#if defined(BRAFT_REVISION)
        os << BRAFT_REVISION;
#else
        os << "undefined";
#endif
}

static bvar::PassiveStatus<std::string> s_raft_revision(
        "raft_revision", print_revision, NULL);


static pthread_once_t global_init_once = PTHREAD_ONCE_INIT;


// Non-static for unit test
void global_init_once_or_die() {
    if (pthread_once(&global_init_once, global_init_or_die_impl) != 0) {
        PLOG(FATAL) << "Fail to pthread_once";
        exit(-1);
    }
}

class FollowerStableClosure : public LogManager::StableClosure {
public:
    FollowerStableClosure(
            brpc::Controller* cntl,
            const AppendEntriesRequest* request,
            AppendEntriesResponse* response,
            google::protobuf::Closure* done,
            NodeImpl* node,
            int64_t term)
        : _cntl(cntl)
        , _request(request)
        , _response(response)
        , _done(done)
        , _node(node)
        , _term(term)
    {
        _node->AddRef();
    }
    void Run() {
        run();
        delete this;
    }
private:
    ~FollowerStableClosure() {
        if (_node) {
            _node->Release();
        }
    }
    void run() {
        brpc::ClosureGuard done_guard(_done);
        if (!status().ok()) {
            _cntl->SetFailed(status().error_code(), "%s",
                             status().error_cstr());
            return;
        }
        std::unique_lock<raft::raft_mutex_t> lck(_node->_mutex);
        if (_term != _node->_current_term) {
            // The change of term indicates that leader has been changed during
            // appending entries, so we can't respond ok to the old leader
            // because we are not sure if the appended logs would be truncated
            // by the new leader:
            //  - If they won't be truncated and we respond failure to the old
            //    leader, the new leader would know that they are stored in this
            //    peer and they will be eventually committed when the new leader
            //    found that quorum of the cluster have stored.
            //  - If they will be truncated and we responded success to the old
            //    leader, the old leader would possibly regard those entries as
            //    committed (very likely in a 3-nodes cluster) and respond
            //    success to the clients, which would break the rule that
            //    committed entries would never be truncated.
            // So we have to respond failure to the old leader and set the new
            // term to make it stepped down if it didn't.
            _response->set_success(false);
            _response->set_term(_node->_current_term);
            return;
        }
        // It's safe to release lck as we know everything is ok at this point.
        lck.unlock();

        // DON'T touch _node any more
        _response->set_success(true);
        _response->set_term(_term);

        const int64_t committed_index =
                std::min(_request->committed_index(),
                         // ^^^ committed_index is likely less than the
                         // last_log_index
                         _request->prev_log_index() + _request->entries_size()
                         // ^^^ The logs after the appended entries are
                         // untrustable so we can't commit them even if their
                         // indexes are less than request->committed_index()
                        );
        //_ballot_box is thread safe and tolerats disorder.
        _node->_ballot_box->set_last_committed_index(committed_index);
    }

    brpc::Controller* _cntl;
    const AppendEntriesRequest* _request;
    AppendEntriesResponse* _response;
    google::protobuf::Closure* _done;
    NodeImpl* _node;
    int64_t _term;
};

NodeImpl::NodeImpl(){};

NodeImpl::NodeImpl(const GroupId& group_id, const PeerId& peer_id){};

NodeImpl::~NodeImpl(){};

NodeImpl& NodeImpl::getInstance(){
    static NodeImpl instance;
        return instance;
}

int NodeImpl::init(NodeOptions node_options, const GroupId& group_id, const PeerId& peer_id){
    _group_id = group_id;

    global_init_once_or_die();
    _options = NodeOptions(node_options);
    _conf.id = LogId();
    _conf.conf = _options.initial_conf;
    _server_id = peer_id;
    node_options.initial_conf.list_peers(&_peer_list);

    _config_manager = new ConfigurationManager();

    
    // Create _fsm_caller first as log_manager needs it to report error
    _fsm_caller = new FSMCaller();

    // log storage and log manager init
    if (init_log_storage() != 0) {
        LOG(ERROR) << "node " << _group_id << ":" << _server_id
                   << " init_log_storage failed";
        return -1;
    }

    // meta init
    if (init_meta_storage() != 0) {
        LOG(ERROR) << "node " << _group_id << ":" << _server_id
                   << " init_meta_storage failed";
        return -1;
    }

    if (init_fsm_caller(LogId(0, 0)) != 0) {
        LOG(ERROR) << "Fail to init fsm_caller";
        return -1;
    }

    if (bthread::execution_queue_start(&_apply_queue_id, NULL,
                                       execute_applying_tasks, this) != 0) {
        LOG(ERROR) << "node " << _group_id << ":" << _server_id 
                   << " fail to start execution_queue";
        return -1;
    }

    _apply_queue = execution_queue_address(_apply_queue_id);

    if (!_apply_queue) {
        LOG(ERROR) << "node " << _group_id << ":" << _server_id
                   << " fail to address execution_queue";
        return -1;
    }

    // commitment manager init
    _ballot_box = new BallotBox();
    BallotBoxOptions ballot_box_options;
    ballot_box_options.waiter = _fsm_caller;
    ballot_box_options.closure_queue = _closure_queue;
    if (_ballot_box->init(ballot_box_options) != 0) {
        LOG(ERROR) << "node " << _group_id << ":" << _server_id
                   << " init _ballot_box failed";
        return -1;
    }
    
    // init replicator
    ReplicatorGroupOptions rg_options;
    rg_options.heartbeat_timeout_ms = heartbeat_timeout(_options.election_timeout_ms);
    rg_options.election_timeout_ms = _options.election_timeout_ms;
    rg_options.log_manager = _log_manager;
    rg_options.ballot_box = _ballot_box;
    rg_options.node = this;

    _replicator_group.init(NodeId(_group_id, _server_id), rg_options);

    _state = STATE_FOLLOWER;
    
    return 0;
}

int NodeImpl::start(){

    // brpc::Server server;

    // RaftSericeImpl raft_service_impl;
    
    // if (server.AddService(&raft_service_impl, 
    //                         brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
    //         LOG(ERROR) << "Fail to add service";
    //         return -1;
    //     }
        
    CHECK_EQ(0, _election_timer.init(this, 1000));
    CHECK_EQ(0, _vote_timer.init(this, 1000));

    // Start the server.
    // brpc::ServerOptions options;
    // options.idle_timeout_sec = _server_timeout;
    // if (server.Start(_server_id.addr, &options) != 0) {
    //     LOG(ERROR) << "Fail to start EchoServer";
    //     return -1;
    // }

    _election_timer.start();

    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    // server.RunUntilAskedToQuit();

    return 0;
}

struct OnPreVoteRPCDone : public google::protobuf::Closure {
    OnPreVoteRPCDone(const PeerId& peer_id_, const int64_t term_, NodeImpl* node_)
        : peer(peer_id_), term(term_), node(node_) {
            node->AddRef();
    }
    virtual ~OnPreVoteRPCDone() {
        node->Release();
    }

    void Run() {
        do {
            if (cntl.ErrorCode() != 0) {
                LOG(WARNING) << "node " << node->node_id()
                             << " request PreVote from " << peer 
                             << " error: " << cntl.ErrorText();
                break;
            }
            node->handle_pre_vote_response(peer, term, response);
        } while (0);
        delete this;
    }

    PeerId peer;
    int64_t term;
    RequestVoteRequest request;
    RequestVoteResponse response;
    brpc::Controller cntl;
    NodeImpl* node;
};

struct OnRequestVoteRPCDone : public google::protobuf::Closure {
    OnRequestVoteRPCDone(const PeerId& peer_id_, const int64_t term_, NodeImpl* node_)
        : peer(peer_id_), term(term_), node(node_) {
            node->AddRef();
    }
    virtual ~OnRequestVoteRPCDone() {
        node->Release();
    }

    void Run() {
        do {
            if (cntl.ErrorCode() != 0) {
                LOG(WARNING) << "node " << node->node_id()
                             << " received RequestVoteResponse from " << peer 
	                         << " error: " << cntl.ErrorText();
                break;
            }
            node->handle_request_vote_response(peer, term, response);
        } while (0);
        delete this;
    }

    PeerId peer;
    int64_t term;
    RequestVoteRequest request;
    RequestVoteResponse response;
    brpc::Controller cntl;
    NodeImpl* node;
};

int NodeImpl::handle_prevote(const RequestVoteRequest* request, RequestVoteResponse* response){
    LOG(INFO) << "Handling prevote from " << request->server_id() << ", term = " << request->last_log_term() << ", index = " << request->last_log_index() << std::endl;
    
    std::unique_lock<raft::raft_mutex_t> lck(_mutex);

    PeerId candidate_id;
    if (0 != candidate_id.parse(request->server_id())) {
        LOG(WARNING) << "node " << _group_id << ":" << _server_id
                     << " received PreVote from " << request->server_id()
                     << " server_id bad format";
        return EINVAL;
    }

    bool granted = false;

    do {
        if (request->term() < _current_term) {
            // ignore older term
            LOG(INFO) << "node " << _group_id << ":" << _server_id
                      << " ignore PreVote from " << request->server_id()
                      << " in term " << request->term()
                      << " current_term " << _current_term;
            break;
        }

        // get last_log_id outof node mutex
        lck.unlock();
        LogId last_log_id = _log_manager->last_log_id(true);
        lck.lock();
        // pre_vote not need ABA check after unlock&lock

        granted = (LogId(request->last_log_index(), request->last_log_term())
                        >= last_log_id);

        LOG(INFO) << "node " << _group_id << ":" << _server_id
                  << " received PreVote from " << request->server_id()
                  << " in term " << request->term()
                  << " current_term " << _current_term
                  << " granted " << granted;

    } while (0);

    response->set_term(_current_term);
    response->set_granted(granted);

    return 0;
}


int NodeImpl::handle_request_vote(const RequestVoteRequest* request, RequestVoteResponse* response){
    std::unique_lock<raft::raft_mutex_t> lck(_mutex);

    PeerId candidate_id;
    if (0 != candidate_id.parse(request->server_id())) {
        LOG(WARNING) << "node " << _group_id << ":" << _server_id
                     << " received PreVote from " << request->server_id()
                     << " server_id bad format";
        return EINVAL;
    }

    do {
        // check term
        if (request->term() >= _current_term) {
            LOG(INFO) << "node " << _group_id << ":" << _server_id
                      << " received RequestVote from " << request->server_id()
                      << " in term " << request->term()
                      << " current_term " << _current_term;
            // incress current term, change state to follower
            if (request->term() > _current_term) {
                butil::Status status;
                status.set_error(EHIGHERTERMREQUEST, "Raft node receives higher term "
                        "request_vote_request.");
                step_down(request->term(), false, status);
            }
        } else {
            // ignore older term
            LOG(INFO) << "node " << _group_id << ":" << _server_id
                      << " ignore RequestVote from " << request->server_id()
                      << " in term " << request->term()
                      << " current_term " << _current_term;
            break;
        }

        // get last_log_id outof node mutex
        lck.unlock();
        LogId last_log_id = _log_manager->last_log_id(true);
        lck.lock();
        // vote need ABA check after unlock&lock
        if (request->term() != _current_term) {
            LOG(WARNING) << "node " << _group_id << ":" << _server_id
                         << " raise term " << _current_term << " when get last_log_id";
            break;
        }

        bool log_is_ok = (LogId(request->last_log_index(), request->last_log_term())
                          >= last_log_id);
        // save
        if (log_is_ok && _voted_id.is_empty()) {
            butil::Status status;
            status.set_error(EVOTEFORCANDIDATE, "Raft node votes for some candidate, "
                    "step down to restart election_timer.");
            step_down(request->term(), false, status);
            _voted_id = candidate_id;
            //TODO: outof lock
            _meta_storage->set_votedfor(candidate_id);
        }
    } while (0);

    response->set_term(_current_term);
    response->set_granted(request->term() == _current_term && _voted_id == candidate_id);
    return 0;
}

void NodeImpl::apply_task(const Task& task){
    LogEntry* entry = new LogEntry;
    entry->AddRef();
    entry->data.swap(*task.data);
    LogEntryAndClosure m;
    m.entry = entry;
    m.done = task.done;
    m.expected_term = task.expected_term;
    if (_apply_queue->execute(m, &bthread::TASK_OPTIONS_INPLACE, NULL) != 0){
        task.done->status().set_error(EPERM, "Node is down");
        entry->Release();
        return run_closure_in_bthread(task.done);
    }
}


void NodeImpl::prevote(std::unique_lock<raft::raft_mutex_t>* lck){
    // Create a new ballot for pre-vote
    _prevote_ctx.init(_conf.conf, _conf.stable() ? NULL : &_conf.old_conf);

    LOG(INFO)<< _log_manager->last_log_id();

    const LogId lastLogId(_log_manager->last_log_id());

    for (std::set<PeerId>::const_iterator
            iter = _peer_list.begin(); iter != _peer_list.end(); ++iter) {

        if (*iter == _server_id){
            continue;
        }
        brpc::ChannelOptions options;
        options.connection_type = brpc::CONNECTION_TYPE_SINGLE;
        options.max_retry = 0;
        brpc::Channel channel;
        if (0 != channel.Init(iter->addr, &options)) {
            LOG(WARNING) << "channel init failed, addr " << iter->addr;
        }

        OnPreVoteRPCDone* done = new OnPreVoteRPCDone(*iter, _current_term, this);
        done->cntl.set_timeout_ms(500);
        done->request.set_server_id(_server_id.to_string());
        done->request.set_peer_id(iter->to_string());
        done->request.set_term(_current_term + 1); // next term
        done->request.set_last_log_index(lastLogId.index);
        done->request.set_last_log_term(lastLogId.term);
        RaftService_Stub stub(&channel);
        stub.prevote(&done->cntl, &done->request, &done->response, done);
    }
    
    _prevote_ctx.grant(_server_id);
}

void NodeImpl::vote(std::unique_lock<raft::raft_mutex_t>* lck){
    // Create a new ballot for pre-vote
    _vote_ctx.init(_conf.conf, _conf.stable() ? NULL : &_conf.old_conf);

    LOG(INFO)<< _log_manager->last_log_id();

    const LogId lastLogId(_log_manager->last_log_id());
    
}

void NodeImpl::handle_pre_vote_response(const PeerId& peer_id, const int64_t term, RequestVoteResponse response){

    LOG(INFO) << "GOT RESPONSE";

    std::unique_lock<raft::raft_mutex_t> lck(_mutex);

    // check state
    if (_state != STATE_FOLLOWER) {
        LOG(WARNING) << "node " << _group_id << ":" << _server_id
                     << " received invalid PreVoteResponse from " << peer_id
                     << " state not in STATE_FOLLOWER but " << state2str(_state);
        return;
    }
    // check stale response
    if (term != _current_term) {
        LOG(WARNING) << "node " << _group_id << ":" << _server_id
                     << " received stale PreVoteResponse from " << peer_id
                     << " term " << term << " current_term " << _current_term;
        return;
    }
    // check response term
    if (response.term() > _current_term) {
        LOG(WARNING) << "node " << _group_id << ":" << _server_id
                     << " received invalid PreVoteResponse from " << peer_id
                     << " term " << response.term() << " expect " << _current_term;
        butil::Status status;
        status.set_error(EHIGHERTERMRESPONSE, "Raft node receives higher term "
                "pre_vote_response.");
        step_down(response.term(), false, status);
        return;
    }

    LOG(INFO) << "node " << _group_id << ":" << _server_id
              << " received PreVoteResponse from " << peer_id
              << " term " << response.term() << " granted " << response.granted();
    // check if the quorum granted
    if (response.granted()) {
        _prevote_ctx.grant(peer_id);
        if (_prevote_ctx.granted()) {
            elect_self(&lck);
        }
    }
}

void NodeImpl::elect_self(std::unique_lock<raft::raft_mutex_t>* lck) {
    LOG(INFO) << "node " << _group_id << ":" << _server_id
              << " term " << _current_term << " start vote and grant vote self";
    if (!_conf.contains(_server_id)) {
        LOG(WARNING) << "node " << _group_id << ':' << _server_id
                     << " can't do elect_self as it is not in " << _conf.conf;
        return;
    }
    // cancel follower election timer
    if (_state == STATE_FOLLOWER) {
        BRAFT_VLOG << "node " << _group_id << ":" << _server_id
                   << " term " << _current_term << " stop election_timer";
        _election_timer.stop();
    }
    // reset leader_id before vote
    PeerId empty_id;
    butil::Status status;
    status.set_error(ERAFTTIMEDOUT,
            "A follower's leader_id is reset to NULL "
            "as it begins to request_vote.");
    reset_leader_id(empty_id, status);

    _state = STATE_CANDIDATE;
    _current_term++;
    _voted_id = _server_id;

    BRAFT_VLOG << "node " << _group_id << ":" << _server_id
               << " term " << _current_term << " start vote_timer";
    _vote_timer.start();

    _vote_ctx.init(_conf.conf, _conf.stable() ? NULL : &_conf.old_conf);

    int64_t old_term = _current_term;
    // get last_log_id outof node mutex
    lck->unlock();
    const LogId last_log_id = _log_manager->last_log_id(true);
    lck->lock();
    // vote need defense ABA after unlock&lock
    if (old_term != _current_term) {
        // term changed cause by step_down
        LOG(WARNING) << "node " << _group_id << ":" << _server_id
                     << " raise term " << _current_term << " when get last_log_id";
        return;
    }
    std::set<PeerId> peers;
    _conf.list_peers(&peers);

    for (std::set<PeerId>::const_iterator
        iter = peers.begin(); iter != peers.end(); ++iter) {
        if (*iter == _server_id) {
            continue;
        }
        brpc::ChannelOptions options;
        options.connection_type = brpc::CONNECTION_TYPE_SINGLE;
        options.max_retry = 0;
        brpc::Channel channel;
        if (0 != channel.Init(iter->addr, &options)) {
            LOG(WARNING) << "node " << _group_id << ":" << _server_id
                         << " channel init failed, addr " << iter->addr;
            continue;
        }

        OnRequestVoteRPCDone* done = new OnRequestVoteRPCDone(*iter, _current_term, this);
        done->cntl.set_timeout_ms(_options.election_timeout_ms);
        done->request.set_server_id(_server_id.to_string());
        done->request.set_peer_id(iter->to_string());
        done->request.set_term(_current_term);
        done->request.set_last_log_index(last_log_id.index);
        done->request.set_last_log_term(last_log_id.term);

        RaftService_Stub stub(&channel);
        stub.request_vote(&done->cntl, &done->request, &done->response, done);
    }

    //TODO: outof lock
    _meta_storage->set_term_and_votedfor(_current_term, _server_id);
    _vote_ctx.grant(_server_id);
    if (_vote_ctx.granted()) {
        become_leader();
    }
}

void NodeImpl::handle_request_vote_response(const PeerId& peer_id, const int64_t term, RequestVoteResponse response){
    BAIDU_SCOPED_LOCK(_mutex);

    // check state
    if (_state != STATE_CANDIDATE) {
        LOG(WARNING) << "node " << _group_id << ":" << _server_id
                     << " received invalid RequestVoteResponse from " << peer_id
                     << " state not in CANDIDATE but " << state2str(_state);
        return;
    }
    // check stale response
    if (term != _current_term) {
        LOG(WARNING) << "node " << _group_id << ":" << _server_id
                     << " received stale RequestVoteResponse from " << peer_id
                     << " term " << term << " current_term " << _current_term;
        return;
    }
    // check response term
    if (response.term() > _current_term) {
        LOG(WARNING) << "node " << _group_id << ":" << _server_id
                     << " received invalid RequestVoteResponse from " << peer_id
                     << " term " << response.term() << " expect " << _current_term;
        butil::Status status;
        status.set_error(EHIGHERTERMRESPONSE, "Raft node receives higher term "
                "request_vote_response.");
        step_down(response.term(), false, status);
        return;
    }

    LOG(INFO) << "node " << _group_id << ":" << _server_id
              << " received RequestVoteResponse from " << peer_id
              << " term " << response.term() << " granted " << response.granted();

    // check if the quorum granted
    if (response.granted()) {
        _vote_ctx.grant(peer_id);
        if (_vote_ctx.granted()) {
            become_leader();
        }
    }
}

void NodeImpl::become_leader(){    
    
    CHECK(_state == STATE_CANDIDATE);
    LOG(INFO) << "node " << _group_id << ":" << _server_id
              << " term " << _current_term
              << " become leader of group " << _conf.conf
              << " " << _conf.old_conf;
    // cancel candidate vote timer
    _vote_timer.stop();

    _state = STATE_LEADER;
    _leader_id = _server_id;

    _replicator_group.reset_term(_current_term);

    std::set<PeerId> peers;
    _conf.list_peers(&peers);
    for (std::set<PeerId>::const_iterator
            iter = peers.begin(); iter != peers.end(); ++iter) {
        if (*iter == _server_id) {
            continue;
        }

        LOG(INFO) << "node " << _group_id << ":" << _server_id
                  << " term " << _current_term
                  << " add replicator " << *iter;
        //TODO: check return code
        _replicator_group.add_replicator(*iter);
    }

    // init commit manager
    _ballot_box->reset_pending_index(_log_manager->last_log_index() + 1);

    _stepdown_timer.start();
}

void NodeImpl::handle_append_entries_request(brpc::Controller* cntl,
                                        const AppendEntriesRequest* request,
                                        AppendEntriesResponse* response,
                                        google::protobuf::Closure* done,
                                        bool from_append_entries_cache){
    std::vector<LogEntry*> entries;
    entries.reserve(request->entries_size());
    brpc::ClosureGuard done_guard(done);
    std::unique_lock<raft::raft_mutex_t> lck(_mutex);

    // pre set term, to avoid get term in lock
    response->set_term(_current_term);

    PeerId server_id;
    if (0 != server_id.parse(request->server_id())) {
        lck.unlock();
        LOG(WARNING) << "node " << _group_id << ":" << _server_id
                     << " received AppendEntries from " << request->server_id()
                     << " server_id bad format";
        cntl->SetFailed(brpc::EREQUEST,
                        "Fail to parse server_id `%s'",
                        request->server_id().c_str());
        return;
    }

    // check stale term
    if (request->term() < _current_term) {
        const int64_t saved_current_term = _current_term;
        lck.unlock();
        LOG(WARNING) << "node " << _group_id << ":" << _server_id
                     << " ignore stale AppendEntries from " << request->server_id()
                     << " in term " << request->term()
                     << " current_term " << saved_current_term;
        response->set_success(false);
        response->set_term(saved_current_term);
        return;
    }
    
    // check term and state to step down
    check_step_down(request->term(), server_id);   

    if (server_id != _leader_id) {
        LOG(ERROR) << "Another peer " << _group_id << ":" << server_id
                   << " declares that it is the leader at term=" << _current_term 
                   << " which was occupied by leader=" << _leader_id;
        // Increase the term by 1 and make both leaders step down to minimize the
        // loss of split brain
        butil::Status status;
        status.set_error(ELEADERCONFLICT, "More than one leader in the same term."); 
        step_down(request->term() + 1, false, status);
        response->set_success(false);
        response->set_term(request->term() + 1);
        return;
    }

    if (!from_append_entries_cache) {
        // Requests from cache already updated timestamp
        _last_leader_timestamp = butil::monotonic_time_ms();
    }

    const int64_t prev_log_index = request->prev_log_index();
    const int64_t prev_log_term = request->prev_log_term();
    const int64_t local_prev_log_term = _log_manager->get_term(prev_log_index);
    if (local_prev_log_term != prev_log_term) {
        int64_t last_index = _log_manager->last_log_index();
        int64_t saved_term = request->term();
        int     saved_entries_size = request->entries_size();
        std::string rpc_server_id = request->server_id();
        if (!from_append_entries_cache /*&& handle_out_of_order_append_entries(cntl, request, response, done, last_index) */) {
            // It's not safe to touch cntl/request/response/done after this point,
            // since the ownership is tranfered to the cache.
            lck.unlock();
            done_guard.release();
            LOG(WARNING) << "node " << _group_id << ":" << _server_id
                         << " cache out-of-order AppendEntries from " 
                         << rpc_server_id
                         << " in term " << saved_term
                         << " prev_log_index " << prev_log_index
                         << " prev_log_term " << prev_log_term
                         << " local_prev_log_term " << local_prev_log_term
                         << " last_log_index " << last_index
                         << " entries_size " << saved_entries_size;
            return;
        }

        response->set_success(false);
        response->set_term(_current_term);
        response->set_last_log_index(last_index);
        lck.unlock();
        LOG(WARNING) << "node " << _group_id << ":" << _server_id
                     << " reject term_unmatched AppendEntries from " 
                     << request->server_id()
                     << " in term " << request->term()
                     << " prev_log_index " << request->prev_log_index()
                     << " prev_log_term " << request->prev_log_term()
                     << " local_prev_log_term " << local_prev_log_term
                     << " last_log_index " << last_index
                     << " entries_size " << request->entries_size()
                     << " from_append_entries_cache: " << from_append_entries_cache;
        return;
    }

    if (request->entries_size() == 0) {
        response->set_success(true);
        response->set_term(_current_term);
        response->set_last_log_index(_log_manager->last_log_index());
        response->set_readonly(false);
        lck.unlock();
        // see the comments at FollowerStableClosure::run()
        _ballot_box->set_last_committed_index(
                std::min(request->committed_index(),
                         prev_log_index));
        return;
    }

    // Parse request
    butil::IOBuf data_buf;
    data_buf.swap(cntl->request_attachment());
    int64_t index = prev_log_index;
    for (int i = 0; i < request->entries_size(); i++) {
        index++;
        const EntryMeta& entry = request->entries(i);
        if (entry.type() != ENTRY_TYPE_UNKNOWN) {
            LogEntry* log_entry = new LogEntry();
            log_entry->AddRef();
            log_entry->id.term = entry.term();
            log_entry->id.index = index;
            log_entry->type = (EntryType)entry.type();
            if (entry.peers_size() > 0) {
                log_entry->peers = new std::vector<PeerId>;
                for (int i = 0; i < entry.peers_size(); i++) {
                    log_entry->peers->push_back(entry.peers(i));
                }
                CHECK_EQ(log_entry->type, ENTRY_TYPE_CONFIGURATION);
                if (entry.old_peers_size() > 0) {
                    log_entry->old_peers = new std::vector<PeerId>;
                    for (int i = 0; i < entry.old_peers_size(); i++) {
                        log_entry->old_peers->push_back(entry.old_peers(i));
                    }
                }
            } else {
                CHECK_NE(entry.type(), ENTRY_TYPE_CONFIGURATION);
            }
            if (entry.has_data_len()) {
                int len = entry.data_len();
                data_buf.cutn(&log_entry->data, len);
            }
            entries.push_back(log_entry);
        }
    }

    // // check out-of-order cache
    // check_append_entries_cache(index);

    FollowerStableClosure* c = new FollowerStableClosure(
            cntl, request, response, done_guard.release(),
            this, _current_term);
    _log_manager->append_entries(&entries, c);

    // update configuration after _log_manager updated its memory status
    _log_manager->check_and_set_configuration(&_conf);

}

 int NodeImpl::execute_applying_tasks(void* meta, bthread::TaskIterator<LogEntryAndClosure>& iter){
    LOG(INFO) << "Got Task";

    if (iter.is_queue_stopped()) {
        return 0;
    }
    // TODO: the batch size should limited by both task size and the total log
    // size
    const size_t batch_size = FLAGS_raft_apply_batch;
    DEFINE_SMALL_ARRAY(LogEntryAndClosure, tasks, batch_size, 256);
    size_t cur_size = 0;
    NodeImpl* m = (NodeImpl*)meta;
    for (; iter; ++iter) {
        if (cur_size == batch_size) {
            m->apply(tasks, cur_size);
            cur_size = 0;
        }
        tasks[cur_size++] = *iter;
    }
    if (cur_size > 0) {
        m->apply(tasks, cur_size);
    }
    return 0;
 }

void NodeImpl::apply(LogEntryAndClosure tasks[], size_t size) {

    std::vector<LogEntry*> entries;
    entries.reserve(size);
    std::unique_lock<raft::raft_mutex_t> lck(_mutex);

    if (_state != STATE_LEADER) {
        butil::Status st;
        if (_state != STATE_TRANSFERRING) {
            st.set_error(EPERM, "is not leader");
        } else {
            st.set_error(EBUSY, "is transferring leadership");
        }
        lck.unlock();
        BRAFT_VLOG << "node " << _group_id << ":" << _server_id << " can't apply : " << st;
        for (size_t i = 0; i < size; ++i) {
            tasks[i].entry->Release();
            if (tasks[i].done) {
                tasks[i].done->status() = st;
                run_closure_in_bthread(tasks[i].done);
            }
        }
        return;
    }
    for (size_t i = 0; i < size; ++i) {
        if (tasks[i].expected_term != -1 && tasks[i].expected_term != _current_term) {
            LOG(INFO) << "node " << _group_id << ":" << _server_id
                      << " can't apply taks whose expected_term=" << tasks[i].expected_term
                      << " doesn't match current_term=" << _current_term;
            if (tasks[i].done) {
                tasks[i].done->status().set_error(
                        EPERM, "expected_term=%" PRId64 " doesn't match current_term=%" PRId64,
                        tasks[i].expected_term, _current_term);
                run_closure_in_bthread(tasks[i].done);
            }
            tasks[i].entry->Release();
            continue;
        }
        entries.push_back(tasks[i].entry);
        entries.back()->id.term = _current_term;
        entries.back()->type = ENTRY_TYPE_DATA;
        _ballot_box->append_pending_task(_conf.conf,
                                         _conf.stable() ? NULL : &_conf.old_conf,
                                         tasks[i].done);
    }
    _log_manager->append_entries(&entries,
                               new LeaderStableClosure(
                                        NodeId(_group_id, _server_id),
                                        entries.size(),
                                        _ballot_box));
    // update _conf.first
    _log_manager->check_and_set_configuration(&_conf);
}


// in lock
void NodeImpl::check_step_down(const int64_t request_term, const PeerId& server_id) {
    butil::Status status;
    if (request_term > _current_term) {
        status.set_error(ENEWLEADER, "Raft node receives message from "
                "new leader with higher term."); 
        step_down(request_term, false, status);
    } else if (_state != STATE_FOLLOWER) { 
        status.set_error(ENEWLEADER, "Candidate receives message "
                "from new leader with the same term.");
        step_down(request_term, false, status);
    } else if (_leader_id.is_empty()) {
        status.set_error(ENEWLEADER, "Follower receives message "
                "from new leader with the same term.");
        step_down(request_term, false, status); 
    }
    // save current leader
    if (_leader_id.is_empty()) { 
        reset_leader_id(server_id, status);
    }
}

void NodeImpl::handle_election_timeout() {

    std::unique_lock<raft::raft_mutex_t> lck(_mutex);
    // check state
    if (_state != STATE_FOLLOWER) {
        return;
    }

    // check timestamp, skip one cycle check when trigger vote
    if (!_vote_triggered &&
            (butil::monotonic_time_ms() - _last_leader_timestamp) 
            < _options.election_timeout_ms) {
        return;
    }

    _vote_triggered = false;

    // Reset leader as the leader is uncerntain on election timeout.
    PeerId empty_id;
    butil::Status status;
    status.set_error(ERAFTTIMEDOUT, "Lost connection from leader %s",
                                    _leader_id.to_string().c_str());
    reset_leader_id(empty_id, status);

    return prevote(&lck);
    
}


void NodeImpl::handle_vote_timeout() {

    std::unique_lock<raft::raft_mutex_t> lck(_mutex);

    // check state
    if (_state != STATE_CANDIDATE) {
    	return;
    }
    if (FLAGS_raft_step_down_when_vote_timedout) {
        // step down to follower
        LOG(WARNING) << "node " << node_id()
                     << " term " << _current_term
                     << " steps down when reaching vote timeout:"
                        " fail to get quorum vote-granted";
        butil::Status status;
        status.set_error(ERAFTTIMEDOUT, "Fail to get quorum vote-granted");
        step_down(_current_term, false, status);
        prevote(&lck);
    } else {
        // retry vote
        LOG(WARNING) << "node " << _group_id << ":" << _server_id
                     << " term " << _current_term << " retry elect";
        elect_self(&lck);
    }
    
}


void NodeImpl::handle_stepdown_timeout() {
    BAIDU_SCOPED_LOCK(_mutex);

    // check state
    if (_state > STATE_TRANSFERRING) {
        BRAFT_VLOG << "node " << _group_id << ":" << _server_id
            << " term " << _current_term << " stop stepdown_timer"
            << " state is " << state2str(_state);
        return;
    }

    int64_t now = butil::monotonic_time_ms();
    if (!_conf.old_conf.empty()) {
        check_dead_nodes(_conf.old_conf, now);
    }
}

void NodeImpl::check_dead_nodes(const Configuration& conf, int64_t now_ms) {
    std::vector<PeerId> peers;
    conf.list_peers(&peers);
    size_t alive_count = 0;
    Configuration dead_nodes;  // for easily print
    for (size_t i = 0; i < peers.size(); i++) {
        if (peers[i] == _server_id) {
            ++alive_count;
            continue;
        }

        if (now_ms - _replicator_group.last_rpc_send_timestamp(peers[i])
                <= _options.election_timeout_ms) {
            ++alive_count;
            continue;
        }
        dead_nodes.add_peer(peers[i]);
    }
    if (alive_count >= peers.size() / 2 + 1) {
        return;
    }
    LOG(WARNING) << "node " << node_id()
                 << " term " << _current_term
                 << " steps down when alive nodes don't satisfy quorum"
                    " dead_nodes: " << dead_nodes
                 << " conf: " << conf;
    butil::Status status;
    status.set_error(ERAFTTIMEDOUT, "Majority of the group dies");
    step_down(_current_term, false, status);
}

void NodeImpl::on_error(const Error& e){

}

int NodeImpl::increase_term_to(int64_t new_term, const butil::Status& status) {
    BAIDU_SCOPED_LOCK(_mutex);
    if (new_term <= _current_term) {
        return EINVAL;
    }
    step_down(new_term, false, status);
    return 0;
}

int NodeImpl::init_log_storage() {
    CHECK(_fsm_caller);
    if (_options.log_storage) {
        _log_storage = _options.log_storage;
    } else {
        _log_storage = LogStorage::create(_options.log_uri);
    }
    if (!_log_storage) {
        LOG(ERROR) << "node " << _group_id << ":" << _server_id
                   << " find log storage failed, uri " << _options.log_uri;
        return -1;
    }
    _log_manager = new LogManager();
    LogManagerOptions log_manager_options;
    log_manager_options.log_storage = _log_storage;
    log_manager_options.configuration_manager = _config_manager;
    log_manager_options.fsm_caller = _fsm_caller;
    return _log_manager->init(log_manager_options);
}

int NodeImpl::init_meta_storage() {
    int ret = 0;

    do {
        _meta_storage = RaftMetaStorage::create(_options.raft_meta_uri);
        if (!_meta_storage) {
            LOG(WARNING) << "node " << _group_id << ":" << _server_id
                << " find meta storage failed, uri " << _options.raft_meta_uri;
            ret = ENOENT;
            break;
        }

        ret = _meta_storage->init();
        if (ret != 0) {
            LOG(WARNING) << "node " << _group_id << ":" << _server_id
                         << " init meta storage failed, uri " << _options.raft_meta_uri
                         << " ret " << ret;
            break;
        }

        _current_term = _meta_storage->get_term();
        ret = _meta_storage->get_votedfor(&_voted_id);
        if (ret != 0) {
            LOG(WARNING) << "node " << _group_id << ":" << _server_id
                         << " meta storage get_votedfor failed, uri " 
                         << _options.raft_meta_uri << " ret " << ret;
            break;
        }
    } while (0);

    return ret;
}

int NodeImpl::init_fsm_caller(const LogId& bootstrap_id) {
    CHECK(_fsm_caller);
    _closure_queue = new ClosureQueue(_options.usercode_in_pthread);
    // fsm caller init, node AddRef in init
    FSMCallerOptions fsm_caller_options;
    fsm_caller_options.usercode_in_pthread = _options.usercode_in_pthread;
    this->AddRef();
    // fsm_caller_options.after_shutdown =
    //     brpc::NewCallback<NodeImpl*>(after_shutdown, this);
    fsm_caller_options.log_manager = _log_manager;
    fsm_caller_options.fsm = _options.fsm;
    fsm_caller_options.closure_queue = _closure_queue;
    fsm_caller_options.node = this;
    fsm_caller_options.bootstrap_id = bootstrap_id;
    const int ret = _fsm_caller->init(fsm_caller_options);
    if (ret != 0) {
        delete fsm_caller_options.after_shutdown;
        this->Release();
    }
    return ret;
}

void NodeImpl::step_down(const int64_t term, bool wakeup_a_candidate, 
                         const butil::Status& status) {
    // reset leader_id 
    PeerId empty_id;
    reset_leader_id(empty_id, status);

    // delete timer and something else
    if (_state == STATE_CANDIDATE) {
        _vote_timer.stop();
    } else if (_state <= STATE_TRANSFERRING) {
        _stepdown_timer.stop();

        _ballot_box->clear_pending_tasks();
    }

    // soft state in memory
    _state = STATE_FOLLOWER;

    _election_timer.start();

    // meta state
    if (term > _current_term) {
        _current_term = term;
        _voted_id.reset();
        //TODO: outof lock
        _meta_storage->set_term_and_votedfor(term, _voted_id);
    }

}

void NodeImpl::reset_leader_id(const PeerId& new_leader_id, 
        const butil::Status& status) {
    if (new_leader_id.is_empty()) {
        _leader_id.reset();
    } else {
        _leader_id = new_leader_id;
    }
}

void NodeImpl::shutdown(Closure* done) {
    // Note: shutdown is probably invoked more than once, make sure this method
    // is idempotent
    {
        BAIDU_SCOPED_LOCK(_mutex);

        LOG(INFO) << "node " << _group_id << ":" << _server_id << " shutdown,"
            " current_term " << _current_term << " state " << state2str(_state);

        if (_state < STATE_SHUTTING) {
            // if it is leader, set the wakeup_a_candidate with true,
            // if it is follower, call on_stop_following in step_down
            if (_state <= STATE_FOLLOWER) {
                butil::Status status;
                status.set_error(ESHUTDOWN, "Raft node is going to quit.");
                step_down(_current_term, _state == STATE_LEADER, status);
            }

            // change state to shutdown
            _state = STATE_SHUTTING;

            // Destroy all the timer
            _election_timer.destroy();
            _vote_timer.destroy();
            _stepdown_timer.destroy();

            // stop replicator and fsm_caller wait
            if (_log_manager) {
                _log_manager->shutdown();
            }

            // step_down will call _commitment_manager->clear_pending_applications(),
            // this can avoid send LogEntry with closure to fsm_caller.
            // fsm_caller shutdown will not leak user's closure.
            if (_fsm_caller) {
                _fsm_caller->shutdown();
            }
        }

        if (_state != STATE_SHUTDOWN) {
            // This node is shutting, push done into the _shutdown_continuations
            // and after_shutdown would invoked this callbacks.
            if (done) {
                _shutdown_continuations.push_back(done);
            }
            return;
        }
    }  // out of _mutex;

    // This node is down, it's ok to invoke done right now. Don't inovke this
    // inplace to avoid the dead lock issue when done->Run() is going to acquire
    // a mutex which is already held by the caller
    if (done) {
        run_closure_in_bthread(done);
    }
}

void NodeImpl::join() {
    if (_fsm_caller) {
        _fsm_caller->join();
    }
}

// Timers
int NodeTimer::init(NodeImpl* node, int timeout_ms) {
    BRAFT_RETURN_IF(RepeatedTimerTask::init(timeout_ms) != 0, -1);
    _node = node;
    node->AddRef();
    return 0;
}

void NodeTimer::on_destroy() {
    if (_node) {
        _node->Release();
        _node = NULL;
    }
}

void ElectionTimer::run() {
    _node->handle_election_timeout();
}

inline int random_timeout(int timeout_ms) {
    int32_t delta = std::min(timeout_ms, 1000);
    return butil::fast_rand_in(timeout_ms, timeout_ms + delta);
}

int ElectionTimer::adjust_timeout_ms(int timeout_ms) {
    return random_timeout(timeout_ms);
}

void VoteTimer::run() {
    _node->handle_vote_timeout();
}

int VoteTimer::adjust_timeout_ms(int timeout_ms) {
    return random_timeout(timeout_ms);
}

void StepdownTimer::run() {
    _node->handle_stepdown_timeout();
}
