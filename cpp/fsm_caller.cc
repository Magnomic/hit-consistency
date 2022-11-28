#include <butil/logging.h>
#include "raft.h"
#include "log_manager.h"
#include "node.h"
#include "util.h"
#include "raft_message.pb.h"
#include "log_entry.h"
#include "errno.pb.h"
#include "node.h"

#include "fsm_caller.h"
#include <bthread/unstable.h>


static bvar::CounterRecorder g_commit_tasks_batch_counter(
        "raft_commit_tasks_batch_counter");

FSMCaller::FSMCaller()
    : _log_manager(NULL)
    , _fsm(NULL)
    , _closure_queue(NULL)
    , _last_applied_index(0)
    , _last_applied_term(0)
    , _after_shutdown(NULL)
    , _node(NULL)
    , _cur_task(IDLE)
    , _applying_index(0)
    , _queue_started(false)
{
}

FSMCaller::~FSMCaller() {
    CHECK(_after_shutdown == NULL);
}

int FSMCaller::run(void* meta, bthread::TaskIterator<ApplyTask>& iter) {
    FSMCaller* caller = (FSMCaller*)meta;
    if (iter.is_queue_stopped()) {
        caller->do_shutdown();
        return 0;
    }
    std::deque<int64_t> oo_committed_entries;
    int64_t max_committed_index = -1;
    int64_t counter = 0;
    for (; iter; ++iter) {
        if (iter->type == COMMITTED) {
            if (iter->committed_index > max_committed_index) {
                max_committed_index = iter->committed_index;
                counter++;
            }
            while (!iter->oo_committed_entries.empty()){
                oo_committed_entries.push_back(iter->oo_committed_entries.front());
                // LOG(INFO) << "Max Commit at " << iter->oo_committed_entries.front();
                iter->oo_committed_entries.pop_front();
            }
        } else {
            if (max_committed_index >= 0) {
                caller->_cur_task = COMMITTED;
                caller->do_committed(max_committed_index, iter->oo_committed_entries);
                max_committed_index = -1;
                g_commit_tasks_batch_counter << counter;
                counter = 0;
            }
            switch (iter->type) {
            case COMMITTED:
                CHECK(false) << "Impossible";
                break;
            case LEADER_STOP:
                caller->_cur_task = LEADER_STOP;
                caller->do_leader_stop(*(iter->status));
                delete iter->status;
                break;
            case LEADER_START:
                caller->do_leader_start(iter->term);
                break;
            case START_FOLLOWING:
                caller->_cur_task = START_FOLLOWING;
                caller->do_start_following(*(iter->leader_change_context));
                delete iter->leader_change_context;
                break;
            case STOP_FOLLOWING:
                caller->_cur_task = STOP_FOLLOWING;
                caller->do_stop_following(*(iter->leader_change_context));
                delete iter->leader_change_context;
                break;
            case ERROR:
                caller->_cur_task = ERROR;
                caller->do_on_error((OnErrorClousre*)iter->done);
                break;
            case IDLE:
                CHECK(false) << "Can't reach here";
                break;
            };
        }
    }
    if (max_committed_index >= 0) {
        // LOG(INFO) << "Sequential Commit at " << max_committed_index;
        caller->_cur_task = COMMITTED;
        caller->do_committed(max_committed_index, oo_committed_entries);
        g_commit_tasks_batch_counter << counter;
        counter = 0;
    }
    caller->_cur_task = IDLE;
    return 0;
}

bool FSMCaller::pass_by_status(Closure* done) {
    brpc::ClosureGuard done_guard(done);
    if (!_error.status().ok()) {
        if (done) {
            done->status().set_error(
                        EINVAL, "FSMCaller is in bad status=`%s'",
                                _error.status().error_cstr());
        }
        return false;
    }
    done_guard.release();
    return true;
}

int FSMCaller::init(const FSMCallerOptions &options) {
    if (options.log_manager == NULL || options.fsm == NULL 
            || options.closure_queue == NULL) {
        return EINVAL;
    }
    _log_manager = options.log_manager;
    _fsm = options.fsm;
    _closure_queue = options.closure_queue;
    _after_shutdown = options.after_shutdown;
    _node = options.node;
    _last_applied_index.store(options.bootstrap_id.index,
                              butil::memory_order_relaxed);
    _last_applied_term = options.bootstrap_id.term;
    if (_node) {
        _node->AddRef();
    }
    
    bthread::ExecutionQueueOptions execq_opt;
    execq_opt.bthread_attr = options.usercode_in_pthread 
                             ? BTHREAD_ATTR_PTHREAD
                             : BTHREAD_ATTR_NORMAL;
    if (bthread::execution_queue_start(&_queue_id,
                                   &execq_opt,
                                   FSMCaller::run,
                                   this) != 0) {
        LOG(ERROR) << "fsm fail to start execution_queue";
        return -1;
    }
    _queue_started = true;
    return 0;
}

int FSMCaller::shutdown() {
    if (_queue_started) {
        return bthread::execution_queue_stop(_queue_id);
    }
    return 0;
}

void FSMCaller::do_shutdown() {
    if (_node) {
        _node->Release();
        _node = NULL;
    }
    _fsm->on_shutdown();
    if (_after_shutdown) {
        google::protobuf::Closure* saved_done = _after_shutdown;
        _after_shutdown = NULL;
        // after this point, |this| is likely to be destroyed, don't touch
        // anything
        saved_done->Run();
    }
}

int FSMCaller::on_committed(int64_t committed_index, std::deque<int64_t> oo_committed_entries) {
    ApplyTask t;
    t.type = COMMITTED;
    while (!oo_committed_entries.empty() && oo_committed_entries.front() <= committed_index){
        oo_committed_entries.pop_front();
    }
    t.oo_committed_entries = oo_committed_entries;
    t.committed_index = committed_index;
    return bthread::execution_queue_execute(_queue_id, t);
}

class OnErrorClousre : public Closure {
public:
    OnErrorClousre(const Error& e) : _e(e) {
    }
    const Error& error() { return _e; }
    void Run() {
        delete this;
    }
private:
    ~OnErrorClousre() {}
    Error _e;
};

int FSMCaller::on_error(const Error& e) {
    OnErrorClousre* c = new OnErrorClousre(e);
    ApplyTask t;
    t.type = ERROR;
    t.done = c;
    if (bthread::execution_queue_execute(_queue_id, t, 
                                         &bthread::TASK_OPTIONS_URGENT) != 0) {
        c->Run();
        return -1;
    }
    return 0;
}

void FSMCaller::do_on_error(OnErrorClousre* done) {
    brpc::ClosureGuard done_guard(done);
    set_error(done->error());
}

void FSMCaller::set_error(const Error& e) {
    if (_error.type() != ERROR_TYPE_NONE) {
        // Error has already reported
        return;
    }
    _error = e;
    if (_fsm) {
        _fsm->on_error(_error);
    }
    if (_node) {
        _node->on_error(_error);
    }
}

void FSMCaller::do_committed(int64_t committed_index, std::deque<int64_t> _oo_committed_entries) {
    if (!_error.status().ok()) {
        return;
    }
    int64_t last_applied_index = _last_applied_index.load(
                                        butil::memory_order_relaxed);

    int64_t end_committed_index = committed_index;

    // LOG(INFO) << "_last_applied_index is " << _last_applied_index;
    // last_applied_index is sequential applied idx
    if (last_applied_index >= end_committed_index && _oo_committed_entries.empty()) {
        return;
    }
    std::vector<Closure*> closure;
    std::deque<int64_t> out_indexes;
    /* If st_committed_index = _first_index in closure_queue, we pop them.*/
    CHECK_EQ(0, _closure_queue->pop_closure_until(committed_index, _oo_committed_entries, &closure,
                                                  &out_indexes));

    IteratorImpl iter_impl(_fsm, _log_manager, &closure,
                 last_applied_index, end_committed_index, &_applying_index, &out_indexes);
    for (; iter_impl.is_good();) {
        if (iter_impl.entry()->type != ENTRY_TYPE_DATA) {
            if (iter_impl.entry()->type == ENTRY_TYPE_CONFIGURATION) {
                if (iter_impl.entry()->old_peers == NULL) {
                    // Joint stage is not supposed to be noticeable by end users.
                    _fsm->on_configuration_committed(
                            Configuration(*iter_impl.entry()->peers),
                            iter_impl.entry()->id.index);
                }
            }
            // For other entries, we have nothing to do besides flush the
            // pending tasks and run this closure to notify the caller that the
            // entries before this one were successfully committed and applied.
            if (iter_impl.done()) {
                iter_impl.done()->Run();
            }
            iter_impl.next();
            continue;
        }
        Iterator iter(&iter_impl);
        _fsm->on_apply(iter);
        LOG_IF(ERROR, iter.valid())
                << "Node " << _node->node_id() 
                << " Iterator is still valid, did you return before iterator "
                   " reached the end?";
        // Try move to next in case that we pass the same log twice.
        iter.next();
    }
    if (iter_impl.has_error()) {
        set_error(iter_impl.error());
        iter_impl.run_the_rest_closure_with_error();
    }
    last_applied_index = committed_index;
    for (std::deque<int64_t>::iterator it = out_indexes.begin(); it != out_indexes.end(); it++){
        if (*it - _last_applied_index > _oo_apply_queue.size()){
            _oo_apply_queue.push_back(false);
        } else {
            _oo_apply_queue[*it - _last_applied_index] = true;
        }
    }
    while (_oo_apply_queue.front()){
        _oo_apply_queue.pop_front();
        last_applied_index++;
    }
    
    // LOG(INFO) << "last_applied_index is " << last_applied_index;
    const int64_t last_index = last_applied_index;
    const int64_t last_term = _log_manager->get_term(last_index);
    LogId last_applied_id(last_index, last_term);

    _last_applied_index.store(last_applied_index, butil::memory_order_release);
    _last_applied_term = last_term;
    _log_manager->set_applied_id(last_applied_id);
}


int FSMCaller::on_leader_stop(const butil::Status& status) {
    ApplyTask task;
    task.type = LEADER_STOP;
    butil::Status* on_leader_stop_status = new butil::Status(status);
    task.status = on_leader_stop_status;
    if (bthread::execution_queue_execute(_queue_id, task) != 0) {
        delete on_leader_stop_status;
        return -1;
    }
    return 0;
}

int FSMCaller::on_leader_start(int64_t term) {
    ApplyTask task;
    task.type = LEADER_START;
    task.term = term;
    return bthread::execution_queue_execute(_queue_id, task);
}

void FSMCaller::do_leader_stop(const butil::Status& status) {
    _fsm->on_leader_stop(status);
}

void FSMCaller::do_leader_start(int64_t term) {
    _fsm->on_leader_start(term);
}

int FSMCaller::on_start_following(const LeaderChangeContext& start_following_context) {
    ApplyTask task;
    task.type = START_FOLLOWING;
    LeaderChangeContext* context  = new LeaderChangeContext(start_following_context.leader_id(), 
            start_following_context.term(), start_following_context.status());
    task.leader_change_context = context;
    if (bthread::execution_queue_execute(_queue_id, task) != 0) {
        delete context;
        return -1;
    }
    return 0;
}

int FSMCaller::on_stop_following(const LeaderChangeContext& stop_following_context) {
    ApplyTask task;
    task.type = STOP_FOLLOWING;
    LeaderChangeContext* context = new LeaderChangeContext(stop_following_context.leader_id(), 
            stop_following_context.term(), stop_following_context.status());
    task.leader_change_context = context;
    if (bthread::execution_queue_execute(_queue_id, task) != 0) {
        delete context;
        return -1;
    }
    return 0;
}

void FSMCaller::do_start_following(const LeaderChangeContext& start_following_context) {
    _fsm->on_start_following(start_following_context);
}

void FSMCaller::do_stop_following(const LeaderChangeContext& stop_following_context) {
    _fsm->on_stop_following(stop_following_context);
}

void FSMCaller::describe(std::ostream &os, bool use_html) {
    const char* newline = (use_html) ? "<br>" : "\n";
    TaskType cur_task = _cur_task;
    const int64_t applying_index = _applying_index.load(
                                    butil::memory_order_relaxed);
    os << "state_machine: ";
    switch (cur_task) {
    case IDLE:
        os << "Idle";
        break;
    case COMMITTED:
        os << "Applying log_index=" << applying_index;
        break;
    case SNAPSHOT_SAVE:
        os << "Saving snapshot";
        break;
    case SNAPSHOT_LOAD:
        os << "Loading snapshot";
        break;
    case ERROR:
        os << "Notifying error";
        break;
    case LEADER_STOP:
        os << "Notifying leader stop";
        break;
    case LEADER_START:
        os << "Notifying leader start";
        break;
    case START_FOLLOWING:
        os << "Notifying start following";
        break;
    case STOP_FOLLOWING:
        os << "Notifying stop following";
        break;
    }
    os << newline;
}

int64_t FSMCaller::applying_index() const {
    TaskType cur_task = _cur_task;
    if (cur_task != COMMITTED) {
        return 0;
    } else {
        return _applying_index.load(butil::memory_order_relaxed);
    }
}

void FSMCaller::join() {
    if (_queue_started) {
        bthread::execution_queue_join(_queue_id);
        _queue_started = false;
    }
}

IteratorImpl::IteratorImpl(StateMachine* sm, LogManager* lm,
                          std::vector<Closure*> *closure, 
                          int64_t last_applied_index, 
                          int64_t committed_index,
                          butil::atomic<int64_t>* applying_index,
                          std::deque<int64_t> *index_list)
        : _sm(sm)
        , _lm(lm)
        , _closure(closure)
        , _committed_index(committed_index)
        , _last_applied_index(last_applied_index)
        , _cur_entry(NULL)
        , _it(-1)
        , _actural_index(0)
        , _applying_index(applying_index)
        , _index_list(index_list)
{ next(); }

void IteratorImpl::next() {
    if (_cur_entry) {
        _cur_entry->Release();
        _cur_entry = NULL;
    }
    if (_closure->empty() && _last_applied_index < _committed_index){
        _index_list->push_back(++_last_applied_index);
    }
    if (++_it >= (int64_t)_index_list->size()){
        _it = -1;
        return;
    }
    _actural_index = (*_index_list)[_it];
    // LOG(INFO) << "_actural_index = " << _actural_index << std::endl
    //         << "_committed_index = " << _committed_index << std::endl
    //         << "_index_list.size = " << _index_list->size() << std::endl
    //         << "_closure.size = " << _closure->size();
    //  _index_list->pop_front();
    _cur_entry = _lm->get_entry(_actural_index);
    if (_cur_entry == NULL) {
        _error.set_type(ERROR_TYPE_LOG);
        _error.status().set_error(-1,
                "Fail to get entry at index=%" PRId64
                " while committed_index=%" PRId64,
                _actural_index, _committed_index);
    }
    _applying_index->store(_actural_index, butil::memory_order_relaxed);
}

Closure* IteratorImpl::done() const {
    // LOG(INFO) << "pop " << _it << " th done" << (*_closure)[_it] ;
    if (_closure->empty()){
        return NULL;
    }
    return (*_closure)[_it];
}

void IteratorImpl::set_error_and_rollback(
            size_t ntail, const butil::Status* st) {
    if (ntail == 0) {
        CHECK(false) << "Invalid ntail=" << ntail;
        return;
    }
    // if (_cur_entry == NULL || _cur_entry->type != ENTRY_TYPE_DATA) {
    //     _cur_index -= ntail;
    // } else {
    //     _cur_index -= (ntail - 1);
    // }
    if (_cur_entry) {
        _cur_entry->Release();
        _cur_entry = NULL;
    }
    _error.set_type(ERROR_TYPE_STATE_MACHINE);
    _error.status().set_error(ESTATEMACHINE, 
            "StateMachine meet critical error when applying one "
            " or more tasks since index=%" PRId64 ", %s", _it,
            (st ? st->error_cstr() : "none"));
}

void IteratorImpl::run_the_rest_closure_with_error() {
    for (int64_t i = _it;
            i <= _index_list->size(); ++i) {
        Closure* done = (*_closure)[i];
        if (done) {
            done->status() = _error.status();
            run_closure_in_bthread(done);
        }
    }
}