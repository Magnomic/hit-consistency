#ifndef _node_H_
#define _node_H_

#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <functional>
#include <set>

#include <butil/logging.h>
#include <butil/atomic_ref_count.h>
#include <butil/memory/ref_counted.h>
#include <butil/iobuf.h>
#include <brpc/server.h>
#include <brpc/channel.h>
#include <brpc/callback.h>

#include "raft_message.pb.h"
#include "errno.pb.h"

#include "raft_service.h"
#include "ballot.h"
#include "log.h"
#include "raft.h"
#include "configuration_manager.h"
#include "replicator.h"
#include "configuration.h"
#include "repeated_timer_task.h"
#include "log_manager.h"
#include "ballot_box.h"

using hit_consistency::RaftService;
using hit_consistency::RaftService_Stub;
using hit_consistency::RequestVoteRequest;
using hit_consistency::RequestVoteResponse;
using hit_consistency::EntryType;
using hit_consistency::EHIGHERTERMRESPONSE;
using hit_consistency::EVOTEFORCANDIDATE;
using hit_consistency::EHIGHERTERMREQUEST;
using hit_consistency::ERAFTTIMEDOUT;
using hit_consistency::ENEWLEADER;
using hit_consistency::ELEADERCONFLICT;

enum State {
    STATE_LEADER = 1,
    STATE_TRANSFERRING = 2,
    STATE_CANDIDATE = 3,
    STATE_FOLLOWER = 4,
    STATE_ERROR = 5,
    STATE_UNINITIALIZED = 6,
    STATE_SHUTTING = 7,
    STATE_SHUTDOWN = 8,
    STATE_END,
};


inline const char* state2str(State state) {
    const char* str[] = {"LEADER", "TRANSFERRING", "CANDIDATE", "FOLLOWER", 
                         "ERROR", "UNINITIALIZED", "SHUTTING", "SHUTDOWN", };
    if (state < STATE_END) {
        return str[(int)state - 1];
    } else {
        return "UNKNOWN";
    }
}


class NodeImpl;

class NodeTimer : public RepeatedTimerTask {
    public:
        NodeTimer() : _node(NULL) {}
        virtual ~NodeTimer() {}
        int init(NodeImpl* node, int timeout_ms);
        virtual void run() = 0;
    protected:
        void on_destroy();
        NodeImpl* _node;
};

class ElectionTimer : public NodeTimer {
    protected:
        void run();
        int adjust_timeout_ms(int timeout_ms);
};

class VoteTimer : public NodeTimer {
    protected:
        void run();
        int adjust_timeout_ms(int timeout_ms);
};


class StepdownTimer : public NodeTimer {
    protected:
        void run();
};

class BAIDU_CACHELINE_ALIGNMENT NodeImpl : public butil::RefCountedThreadSafe<NodeImpl>{

friend class FollowerStableClosure;

    private:

        friend class butil::RefCountedThreadSafe<NodeImpl>;

        std::set<PeerId> _peer_list;

        GroupId _group_id;
        
        PeerId _server_id;

        PeerId _leader_id;
        
        PeerId _voted_id;

        int _current_term;

        BallotBox* _ballot_box;

        Ballot _prevote_ctx;

        Ballot _vote_ctx;
        
        ConfigurationEntry _conf;

        NodeOptions _options;
   
        int64_t _last_leader_timestamp;

        State _state;

        int _server_timeout;

        bool _vote_triggered;

        ElectionTimer _election_timer;

        VoteTimer _vote_timer;
        
        StepdownTimer _stepdown_timer;

        raft::raft_mutex_t _mutex;

        LogManager* _log_manager;
        
        LogStorage* _log_storage;

        RaftMetaStorage* _meta_storage;

        ConfigurationManager* _config_manager;
  
        ClosureQueue* _closure_queue;

        ReplicatorGroup _replicator_group;
        
        std::vector<Closure*> _shutdown_continuations;

        bthread::ExecutionQueueId<LogEntryAndClosure> _apply_queue_id;

        bthread::ExecutionQueue<LogEntryAndClosure>::scoped_ptr_t _apply_queue;

        NodeImpl();
        
        ~NodeImpl();

        NodeImpl(const NodeImpl&);

        NodeImpl& operator=(const NodeImpl&);

    public:        

        NodeImpl(const GroupId& group_id, const PeerId& peer_id);

        NodeId node_id() const {
            return NodeId(_group_id, _server_id);
        }
        static NodeImpl& getInstance();

        int init(NodeOptions node_options, const GroupId& group_id, const PeerId& peer_id);

        int start();

        void apply_task(const Task& task);

        void prevote(std::unique_lock<raft::raft_mutex_t>* lck);

        void vote(std::unique_lock<raft::raft_mutex_t>* lck);

        int handle_prevote(const RequestVoteRequest* request, RequestVoteResponse* response);

        int handle_request_vote(const RequestVoteRequest* request, RequestVoteResponse* response);

        void handle_pre_vote_response(const PeerId& peer_id_, const int64_t term_, RequestVoteResponse response);

        void handle_request_vote_response(const PeerId& peer_id_, const int64_t term_, RequestVoteResponse response);

        void handle_append_entries_request(brpc::Controller* cntl,
                                             const AppendEntriesRequest* request,
                                             AppendEntriesResponse* response,
                                             google::protobuf::Closure* done,
                                             bool from_append_entries_cache);

        void reset_leader_id(const PeerId& new_leader_id, const butil::Status& status);

        void step_down(const int64_t term, bool wakeup_a_candidate, const butil::Status& status);

        void check_dead_nodes(const Configuration& conf, int64_t now_ms);

        void elect_self(std::unique_lock<raft::raft_mutex_t>* lck);

        void become_leader();

        void handle_election_timeout();

        void handle_vote_timeout();

        void handle_stepdown_timeout();
        
        void on_error(const Error& e);

        int increase_term_to(int64_t new_term, const butil::Status& status);

        int init_log_storage();

        int init_meta_storage();

        void check_step_down(const int64_t request_term, const PeerId& server_id);

        static int execute_applying_tasks(void* meta, bthread::TaskIterator<LogEntryAndClosure>& iter);

        void apply(LogEntryAndClosure tasks[], size_t size);
};
#endif 