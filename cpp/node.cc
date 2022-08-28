#include "node.h"

NodeImpl::NodeImpl(){};

NodeImpl::~NodeImpl(){};

NodeImpl& NodeImpl::getInstance(){
    static NodeImpl instance;
        return instance;
}

int NodeImpl::init(std::string server_addr, int server_port, int server_timeout){
    butil::EndPoint point;
    if (!server_addr.empty()) {
        if (butil::str2endpoint(server_addr.c_str(), server_port, &point) < 0) {
            LOG(ERROR) << "Invalid listen address:" << server_addr;
            return -1;
        }
    } else {
        point = butil::EndPoint(butil::IP_ANY, server_port);
    }
    _server_timeout = server_timeout;
    PeerId peer_id(point);
    _server_id = peer_id;
    NodeId _id("default_group", peer_id);
    _peer_list.insert(peer_id);

    butil::EndPoint another_point(butil::EndPoint(butil::IP_ANY, (server_port + 1) % 2 + 8000));
    PeerId another_peer_id(another_point);
    _peer_list.insert(another_peer_id);

    return 0;
}

int NodeImpl::start(){

    brpc::Server server;

    RaftSericeImpl raft_service_impl;
    
    if (server.AddService(&raft_service_impl, 
                            brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
            LOG(ERROR) << "Fail to add service";
            return -1;
        }
    CHECK_EQ(0, _election_timer.init(this, 1000));
    
    // Start the server.
    brpc::ServerOptions options;
    options.idle_timeout_sec = _server_timeout;
    if (server.Start(_server_id.addr, &options) != 0) {
        LOG(ERROR) << "Fail to start EchoServer";
        return -1;
    }

    _election_timer.start();

    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    server.RunUntilAskedToQuit();

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

int NodeImpl::handle_prevote(const RequestVoteRequest* request, RequestVoteResponse* response){
    LOG(INFO) << "Handling prevote from " << request->server_id();
    response->set_term(request->term() + 1);
    return 0;
}


void NodeImpl::prevote(std::unique_lock<raft::raft_mutex_t>* lck){
    LOG(INFO) << "prevote invoked from " << _server_id;

    int _current_term(0);
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
        done->request.set_last_log_index(1);
        done->request.set_last_log_term(0);
        LOG(INFO) << "sending prevote req to " << *iter << std::endl;
        RaftService_Stub stub(&channel);
        stub.prevote(&done->cntl, &done->request, &done->response, done);
    }
    
}

void NodeImpl::handle_pre_vote_response(const PeerId& peer_id_, const int64_t term_, RequestVoteResponse response){

}

void NodeImpl::handle_election_timeout() {

    std::unique_lock<raft::raft_mutex_t> lck(_mutex);

    return prevote(&lck);
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

