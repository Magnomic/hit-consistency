#ifndef BRAFT_RAFT_H
#define BRAFT_RAFT_H

#include <string>

#include <butil/logging.h>
#include <butil/iobuf.h>
#include <butil/status.h>
#include "configuration.h"
#include "enum.pb.h"
#include "errno.pb.h"
#include "raft_meta.h"


using hit_consistency::ERROR_TYPE_NONE;
using hit_consistency::ERROR_TYPE_SNAPSHOT;
using hit_consistency::ERROR_TYPE_STATE_MACHINE;
using hit_consistency::ERROR_TYPE_STABLE;
using hit_consistency::ERROR_TYPE_LOG;
using hit_consistency::ErrorType;

template <typename T> class scoped_refptr;

class StateMachine;

class Closure : public google::protobuf::Closure {
public:
    butil::Status& status() { return _st; }
    const butil::Status& status() const { return _st; }
    
private:
    butil::Status _st;
};


struct LogEntryAndClosure {
    LogEntry* entry;
    int64_t dependency_id;
    Closure* done;
    int64_t expected_term;
};

// Describe a specific error
class Error {
public:
    Error() : _type(ERROR_TYPE_NONE) {}
    Error(const Error& e) : _type(e._type), _st(e._st) {}
    ErrorType type() const { return _type; }
    const butil::Status& status() const { return _st; }
    butil::Status& status() { return _st; }
    void set_type(ErrorType type) { _type = type; }

    Error& operator=(const Error& rhs) {
        _type = rhs._type;
        _st = rhs._st;
        return *this;
    }
private:
    // Intentionally copyable
    ErrorType _type;
    butil::Status _st;
};

struct NodeOptions {

    // A follower would become a candidate if it doesn't receive any message 
    // from the leader in |election_timeout_ms| milliseconds
    // Default: 1000 (1s)
    int election_timeout_ms; //follower to candidate timeout

    // A snapshot saving would be triggered every |snapshot_interval_s| seconds
    // if this was reset as a positive number
    // If |snapshot_interval_s| <= 0, the time based snapshot would be disabled.
    //
    // Default: 3600 (1 hour)
    int snapshot_interval_s;

    // We will regard a adding peer as caught up if the margin between the
    // last_log_index of this peer and the last_log_index of leader is less than
    // |catchup_margin|
    //
    // Default: 1000
    int catchup_margin;

    // If node is starting from a empty environment (both LogStorage and
    // SnapshotStorage are empty), it would use |initial_conf| as the
    // configuration of the group, otherwise it would load configuration from
    // the existing environment.
    //
    // Default: A empty group
    Configuration initial_conf;

    
    // The specific StateMachine implemented your business logic, which must be
    // a valid instance.
    StateMachine* fsm;

    // The specific LogStorage implemented at the bussiness layer, which should be a valid
    // instance, otherwise use SegmentLogStorage by default.
    //
    // Default: null
    LogStorage* log_storage;

    // If |node_owns_fsm| is true. |fms| would be destroyed when the backing
    // Node is no longer referenced.
    //
    // Default: false
    bool node_owns_fsm;

    // If |node_owns_log_storage| is true. |log_storage| would be destroyed when the backing
    // Node is no longer referenced.
    //
    // Default: true
    bool node_owns_log_storage;

    // Run the user callbacks and user closures in pthread rather than bthread
    // 
    // Default: false
    bool usercode_in_pthread;

    // Describe a specific LogStorage in format ${type}://${parameters}
    std::string log_uri;

    // Describe a specific RaftMetaStorage in format ${type}://${parameters}
    std::string raft_meta_uri;

    // Describe a specific SnapshotStorage in format ${type}://${parameters}
    std::string snapshot_uri;

    // If enable, we will filter duplicate files before copy remote snapshot,
    // to avoid useless transmission. Two files in local and remote are duplicate,
    // only if they has the same filename and the same checksum (stored in file meta).
    // Default: false
    bool filter_before_copy_remote;

    // If true, RPCs through raft_cli will be denied.
    // Default: false
    bool disable_cli;

    // Construct a default instance
    NodeOptions();
};

inline NodeOptions::NodeOptions() 
    : election_timeout_ms(1000)
    , snapshot_interval_s(3600)
    , catchup_margin(1000)
    , fsm(NULL)
    , node_owns_fsm(false)
    , node_owns_log_storage(true)
    , usercode_in_pthread(false)
    , filter_before_copy_remote(false)
    , disable_cli(false)
    , log_storage(NULL)
{}

inline const char* errortype2str(ErrorType t) {
    switch (t) {
    case ERROR_TYPE_NONE:
        return "None";
    case ERROR_TYPE_LOG:
        return "LogError";
    case ERROR_TYPE_STABLE:
        return "StableError";
    case ERROR_TYPE_SNAPSHOT:
        return "SnapshotError";
    case hit_consistency::ERROR_TYPE_STATE_MACHINE:
        return "StateMachineError";
    }
    return "Unknown";
}

inline std::ostream& operator<<(std::ostream& os, const Error& e) {
    os << "{type=" << errortype2str(e.type()) 
       << ", error_code=" << e.status().error_code()
       << ", error_text=`" << e.status().error_cstr()
       << "'}";
    return os;
}

// Basic message structure of libraft
struct Task {
    Task() : data(NULL), done(NULL), expected_term(-1), dependency_id(0) {}

    // The data applied to StateMachine
    butil::IOBuf* data;

    // Continuation when the data is applied to StateMachine or error occurs.
    Closure* done;

    // Reject this task if expected_term doesn't match the current term of
    // this Node if the value is not -1
    // Default: -1
    int64_t expected_term;

    int64_t dependency_id;
};

class NodeImpl;

class IteratorImpl;

class Iterator {
    DISALLOW_COPY_AND_ASSIGN(Iterator);
public:
    // Move to the next task.
    void next();

    // Return a unique and monotonically increasing identifier of the current 
    // task:
    //  - Uniqueness guarantees that committed tasks in different peers with 
    //    the same index are always the same and kept unchanged.
    //  - Monotonicity guarantees that for any index pair i, j (i < j), task 
    //    at index |i| must be applied before task at index |j| in all the 
    //    peers from the group.
    int64_t index() const;

    // Returns the term of the leader which to task was applied to.
    int64_t term() const;

    // Return the data whose content is the same as what was passed to
    // Node::apply in the leader node.
    const butil::IOBuf& data() const;

    // If done() is non-NULL, you must call done()->Run() after applying this
    // task no matter this operation succeeds or fails, otherwise the
    // corresponding resources would leak.
    //
    // If this task is proposed by this Node when it was the leader of this 
    // group and the leadership has not changed before this point, done() is 
    // exactly what was passed to Node::apply which may stand for some 
    // continuation (such as respond to the client) after updating the 
    // StateMachine with the given task. Otherweise done() must be NULL.
    Closure* done() const;

    // Return true this iterator is currently references to a valid task, false
    // otherwise, indicating that the iterator has reached the end of this
    // batch of tasks or some error has occurred
    bool valid() const;

    // Invoked when some critical error occurred. And we will consider the last 
    // |ntail| tasks (starting from the last iterated one) as not applied. After
    // this point, no further changes on the StateMachine as well as the Node 
    // would be allowed and you should try to repair this replica or just drop 
    // it.
    //
    // If |st| is not NULL, it should describe the detail of the error.
    void set_error_and_rollback(size_t ntail = 1, const butil::Status* st = NULL);

private:
friend class FSMCaller;
    Iterator(IteratorImpl* impl) : _impl(impl) {}
    ~Iterator() {};

    // The ownership of _impl belongs to FSMCaller;
    IteratorImpl* _impl;
};


// This class encapsulates the parameter of on_start_following and on_stop_following interfaces.
class LeaderChangeContext {
    DISALLOW_COPY_AND_ASSIGN(LeaderChangeContext);
public:
    LeaderChangeContext(const PeerId& leader_id, int64_t term, const butil::Status& status)
        : _leader_id(leader_id)
        , _term(term) 
        , _st(status)
    {};
    // for on_start_following, the leader_id and term are of the new leader;
    // for on_stop_following, the leader_id and term are of the old leader.
    const PeerId& leader_id() const { return _leader_id; }
    int64_t term() const { return _term; }
    // return the information about why on_start_following or on_stop_following is called.
    const butil::Status& status() const { return _st; }
        
private:
    PeerId _leader_id;
    int64_t _term;
    butil::Status _st;
};

inline std::ostream& operator<<(std::ostream& os, const LeaderChangeContext& ctx) {
    os << "{ leader_id=" << ctx.leader_id()
       << ", term=" << ctx.term()
       << ", status=" << ctx.status()
       << "}";
    return os;
}

// |StateMachine| is the sink of all the events of a very raft node.
// Implement a specific StateMachine for your own business logic.
//
// NOTE: All the interfaces are not guaranteed to be thread safe and they are 
// called sequentially, saying that every single operation will block all the 
// following ones.
class StateMachine {
public:
    virtual ~StateMachine();

    // Update the StateMachine with a batch a tasks that can be accessed
    // through |iterator|.
    //
    // Invoked when one or more tasks that were passed to Node::apply have been
    // committed to the raft group (quorum of the group peers have received 
    // those tasks and stored them on the backing storage).
    //
    // Once this function returns to the caller, we will regard all the iterated
    // tasks through |iter| have been successfully applied. And if you didn't
    // apply all the the given tasks, we would regard this as a critical error
    // and report a error whose type is ERROR_TYPE_STATE_MACHINE.
    virtual void on_apply(Iterator& iter) = 0;

    // Invoked once when the raft node was shut down.
    // Default do nothing
    virtual void on_shutdown();
    
    // Invoked when the belonging node becomes the leader of the group at |term|
    // Default: Do nothing
    virtual void on_leader_start(int64_t term);

    // Invoked when this node steps down from the leader of the replication
    // group and |status| describes detailed information
    virtual void on_leader_stop(const butil::Status& status);

    // on_error is called when a critical error was encountered, after this
    // point, no any further modification is allowed to applied to this node
    // until the error is fixed and this node restarts.
    virtual void on_error(const Error& e);

    // Invoked when a configuration has been committed to the group
    virtual void on_configuration_committed(const Configuration& conf);
    virtual void on_configuration_committed(const Configuration& conf, int64_t index);

    // this method is called when a follower stops following a leader and its leader_id becomes NULL,
    // situations including: 
    // 1. handle election_timeout and start pre_vote 
    // 2. receive requests with higher term such as vote_request from a candidate
    // or append_entires_request from a new leader
    // 3. receive timeout_now_request from current leader and start request_vote
    // the parameter stop_following_context gives the information(leader_id, term and status) about the
    // very leader whom the follower followed before.
    // User can reset the node's information as it stops following some leader.
    virtual void on_stop_following(const LeaderChangeContext& ctx);

    // this method is called when a follower or candidate starts following a leader and its leader_id
    // (should be NULL before the method is called) is set to the leader's id,
    // situations including:
    // 1. a candidate receives append_entries from a leader
    // 2. a follower(without leader) receives append_entries from a leader
    // the parameter start_following_context gives the information(leader_id, term and status) about 
    // the very leader whom the follower starts to follow.
    // User can reset the node's information as it starts to follow some leader.
    virtual void on_start_following(const LeaderChangeContext& ctx);
};


#endif //BRAFT_RAFT_H