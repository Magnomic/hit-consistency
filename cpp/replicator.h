#ifndef _replicator_H_
#define _replicator_H_

#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <functional>

#include "raft_message.pb.h"

#include "raft_service.h"

using hit_consistency::RaftService;
using hit_consistency::RequestVoteRequest;
using hit_consistency::RequestVoteResponse;
using hit_consistency::EntryMeta;
using hit_consistency::AppendEntriesRequest;
using hit_consistency::AppendEntriesResponse;
using hit_consistency::TimeoutNowRequest;
using hit_consistency::TimeoutNowResponse;

#include <bthread/bthread.h>                            // bthread_id
#include <brpc/channel.h>                  // brpc::Channel

#include "storage.h"                       // SnapshotStorage
#include "raft.h"                          // Closure
#include "configuration.h"                 // Configuration
#include "raft_message.pb.h"                       // AppendEntriesRequest
#include "log_manager.h"                   // LogManager


class LogManager;
class BallotBox;
class NodeImpl;


struct ReplicatorOptions {
    ReplicatorOptions();
    int* dynamic_heartbeat_timeout_ms;
    int* election_timeout_ms;
    GroupId group_id;
    PeerId server_id;
    PeerId peer_id;
    LogManager* log_manager;
    BallotBox* ballot_box;
    NodeImpl *node;
    int64_t term;
};

typedef uint64_t ReplicatorId;

class CatchupClosure : public Closure {
public:
    virtual void Run() = 0;
protected:
    CatchupClosure()
        : _max_margin(0)
        , _has_timer(false)
        , _error_was_set(false)
    {}
private:
friend class Replicator;
    int64_t _max_margin;
    bthread_timer_t _timer;
    bool _has_timer;
    bool _error_was_set;
    void _run();
};

class BAIDU_CACHELINE_ALIGNMENT Replicator {
public:
    // Called by the leader, otherwise the behavior is undefined
    // Start to replicate the log to the given follower
    static int start(const ReplicatorOptions&, ReplicatorId* id);

    // Called when the leader steps down, otherwise the behavior is undefined
    // Stop replicating
    static int stop(ReplicatorId);

    static int join(ReplicatorId);

    static int64_t last_rpc_send_timestamp(ReplicatorId id);

    // Wait until the margin between |last_log_index| from leader and the peer
    // is less than |max_margin| or error occurs. 
    // |done| can't be NULL and it is called after waiting fnishies.
    static void wait_for_caught_up(ReplicatorId, int64_t max_margin,
                                   const timespec* due_time,
                                   CatchupClosure* done);

    // Tranfer leadership to the very peer if the replicated logs are over
    // |log_index|
    static int transfer_leadership(ReplicatorId id, int64_t log_index);
    static int stop_transfer_leadership(ReplicatorId id);

    // Send TimeoutNowRequest to the very follower to make it become
    // CANDIDATE. And the replicator would stop automatically after the RPC
    // finishes no matter it succes or fails.
    static int send_timeout_now_and_stop(ReplicatorId id, int timeout_ms);

    // Get the next index of this Replica if we know the correct value is
    // Return the correct value on success, 0 otherwise.
    static int64_t get_next_index(ReplicatorId id);

    static void describe(ReplicatorId id, std::ostream& os, bool use_html);

    // Change the readonly config.
    // Return 0 if success, the error code otherwise.
    static int change_readonly_config(ReplicatorId id, bool readonly);

    // Check if a replicator is readonly
    static bool readonly(ReplicatorId id);
    
private:
    enum St {
        IDLE,
        BLOCKING,
        APPENDING_ENTRIES,
        INSTALLING_SNAPSHOT,
    };
    struct Stat {
        St st;
        union {
            int64_t first_log_index;
            int64_t last_log_included;
        };
        union {
            int64_t last_log_index;
            int64_t last_term_included;
        };
    };

    Replicator();
    ~Replicator();

    int _prepare_entry(int offset, EntryMeta* em, butil::IOBuf* data);
    void _wait_more_entries();
    void _send_empty_entries(bool is_hearbeat);
    void _send_entries();
    void _notify_on_caught_up(int error_code, bool);
    int _fill_common_fields(AppendEntriesRequest* request, int64_t prev_log_index,
                            bool is_heartbeat);
    void _block(long start_time_us, int error_code);
    void _start_heartbeat_timer(long start_time_us);
    void _send_timeout_now(bool unlock_id, bool stop_after_finish,
                           int timeout_ms = -1);
    int _transfer_leadership(int64_t log_index);
    void _cancel_append_entries_rpcs();
    void _reset_next_index();
    int64_t _min_flying_index() {
        return _append_entries_in_fly.front().log_index;
    }
    int _change_readonly_config(bool readonly);

    static void _on_rpc_returned(
                ReplicatorId id, brpc::Controller* cntl,
                AppendEntriesRequest* request, 
                AppendEntriesResponse* response,
                int64_t);

    static void _on_heartbeat_returned(
                ReplicatorId id, brpc::Controller* cntl,
                AppendEntriesRequest* request, 
                AppendEntriesResponse* response,
                int64_t);

    static void _on_timeout_now_returned(
                ReplicatorId id, brpc::Controller* cntl,
                TimeoutNowRequest* request, 
                TimeoutNowResponse* response,
                bool stop_after_finish);

    static void _on_timedout(void* arg);
    static void* _send_heartbeat(void* arg);

    static int _on_error(bthread_id_t id, void* arg, int error_code);
    static int _continue_sending(void* arg, int error_code);
    static void* _run_on_caught_up(void*);
    static void _on_catch_up_timedout(void*);
    static void _on_block_timedout(void *arg);
    static void* _on_block_timedout_in_new_thread(void *arg);
    void _destroy();
    void _describe(std::ostream& os, bool use_html);
    bool _is_catchup(int64_t max_margin) {
        // We should wait until install snapshot finish. If the process is throttled,
        // it maybe very slow.
        if (_next_index < _options.log_manager->first_log_index()) {
            return false;
        }
        if (_min_flying_index() - 1 + max_margin
                < _options.log_manager->last_log_index()) {
            return false;
        }
        return true;
    }

private:
    struct FlyingAppendEntriesRpc {
        int64_t log_index;
        int entries_size;
        brpc::CallId call_id;
        FlyingAppendEntriesRpc(int64_t index, int size, brpc::CallId id)
            : log_index(index), entries_size(size), call_id(id) {}
    };
    
    brpc::Channel _sending_channel;
    int64_t _next_index;
    std::deque<bool> _oo_committed_entries;
    int64_t _oo_start_index;
    int64_t _flying_append_entries_size;
    int _consecutive_error_times;
    bool _has_succeeded;
    int64_t _timeout_now_index;
    // the sending time of last successful RPC
    int64_t _last_rpc_send_timestamp;
    int64_t _heartbeat_counter;
    int64_t _append_entries_counter;
    int64_t _install_snapshot_counter;
    int64_t _readonly_index;
    Stat _st;
    std::deque<FlyingAppendEntriesRpc> _append_entries_in_fly;
    std::deque<bool> _status_append_entries_in_fly;
    brpc::CallId _install_snapshot_in_fly;
    brpc::CallId _heartbeat_in_fly;
    brpc::CallId _timeout_now_in_fly;
    LogManager::WaitId _wait_id;
    bool _is_waiter_canceled;
    bthread_id_t _id;
    ReplicatorOptions _options;
    bthread_timer_t _heartbeat_timer;
    CatchupClosure *_catchup_closure;
};

struct ReplicatorGroupOptions {
    ReplicatorGroupOptions();
    int heartbeat_timeout_ms;
    int election_timeout_ms;
    LogManager* log_manager;
    BallotBox* ballot_box;
    NodeImpl* node;
};


// Maintains the replicators attached to all the followers
//  - Invoke reset_term when term changes, which affects the term in the RPC to
//    the adding replicators
//  - Invoke add_replicator for every single follower when nodes becomes LEADER
//    from CANDIDATE
//  - Invoke stop_all when the leader steps down.
//
// Note: The methods of ReplicatorGroup are NOT thread-safe
class ReplicatorGroup {
    public:
        ReplicatorGroup();
        ~ReplicatorGroup();
        int init(const NodeId& node_id, const ReplicatorGroupOptions&);
        
        // Add a replicator attached with |peer|
        // will be a notification when the replicator catches up according to the
        // arguments.
        // NOTE: when calling this function, the replicatos starts to work
        // immediately, annd might call node->step_down which might have race with
        // the caller, you should deal with this situation.
        int add_replicator(const PeerId &peer);
        
        // wait the very peer catchup
        int wait_caughtup(const PeerId& peer, int64_t max_margin,
                        const timespec* due_time, CatchupClosure* done);

        int64_t last_rpc_send_timestamp(const PeerId& peer);

        // Stop all the replicators
        int stop_all();

        int stop_replicator(const PeerId &peer);

        // Reset the term of all to-add replicators.
        // This method is supposed to be called when the very candidate becomes the
        // leader, so we suppose that there are no running replicators.
        // Return 0 on success, -1 otherwise
        int reset_term(int64_t new_term);

        // Reset the interval of heartbeat
        // This method is supposed to be called when the very candidate becomes the
        // leader, use new heartbeat_interval, maybe call vote() reset election_timeout
        // Return 0 on success, -1 otherwise
        int reset_heartbeat_interval(int new_interval_ms);
        
        // Reset the interval of election_timeout for replicator, 
        // used in rpc's set_timeout_ms
        int reset_election_timeout_interval(int new_interval_ms);

        // Returns true if the there's a replicator attached to the given |peer|
        bool contains(const PeerId& peer) const;

        // Transfer leadership to the given |peer|
        int transfer_leadership_to(const PeerId& peer, int64_t log_index);

        // Stop transferring leadership to the given |peer|
        int stop_transfer_leadership(const PeerId& peer);

        // Stop all the replicators except for the one that we think can be the
        // candidate of the next leader, which has the largest `last_log_id' among
        // peers in |current_conf|. 
        // |candidate| would be assigned to a valid ReplicatorId if we found one and
        // the caller is responsible for stopping it, or an invalid value if we
        // found none.
        // Returns 0 on success and -1 otherwise.
        int stop_all_and_find_the_next_candidate(ReplicatorId* candidate,
                                                const ConfigurationEntry& conf);
        
        // Find the follower with the most log entries in this group, which is
        // likely becomes the leader according to the election algorithm of raft.
        // Returns 0 on success and |peer_id| is assigned with the very peer.
        // -1 otherwise.
        int find_the_next_candidate(PeerId* peer_id,
                                    const ConfigurationEntry& conf);

        // List all the existing replicators
        void list_replicators(std::vector<ReplicatorId>* out) const;

        // List all the existing replicators with PeerId
        void list_replicators(std::vector<std::pair<PeerId, ReplicatorId> >* out) const;

        // Change the readonly config for a peer
        int change_readonly_config(const PeerId& peer, bool readonly);

        // Check if a replicator is in readonly
        bool readonly(const PeerId& peer) const;

    private:
        std::map<PeerId, ReplicatorId> _rmap;
        ReplicatorOptions _common_options;
        int _dynamic_timeout_ms;
        int _election_timeout_ms;
};

#endif 