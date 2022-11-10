#ifndef BRAFT_RAFT_STORAGE_H
#define BRAFT_RAFT_STORAGE_H

#include <string>
#include <vector>
#include <gflags/gflags.h>
#include <butil/status.h>
#include <butil/class_name.h>
#include <brpc/extension.h>

#include "configuration.h"
#include "configuration_manager.h"

namespace google {
namespace protobuf {
class Message;
}  // namespace protobuf
}  // namespace google
DECLARE_bool(raft_sync);
DECLARE_bool(raft_sync_meta);
DECLARE_bool(raft_create_parent_directories);

struct LogEntry;

class LogStorage {
public:
    virtual ~LogStorage() {}

    // init logstorage, check consistency and integrity
    virtual int init(ConfigurationManager* configuration_manager) = 0;

    // first log index in log
    virtual int64_t first_log_index() = 0;

    // last log index in log
    virtual int64_t last_log_index() = 0;

    // last log index in log
    virtual int64_t max_log_index() = 0;

    // get logentry by index
    virtual LogEntry* get_entry(const int64_t index) = 0;

    // get logentry's term by index
    virtual int64_t get_term(const int64_t index) = 0;

    // append entries to log
    virtual int append_entry(const LogEntry* entry) = 0;

    // append entries to log, return append success number
    virtual int append_entries(const std::vector<LogEntry*>& entries) = 0;

    // delete logs from storage's head, [first_log_index, first_index_kept) will be discarded
    virtual int truncate_prefix(const int64_t first_index_kept) = 0;

    // delete uncommitted logs from storage's tail, (last_index_kept, last_log_index] will be discarded
    virtual int truncate_suffix(const int64_t last_index_kept) = 0;

    // Drop all the existing logs and reset next log index to |next_log_index|.
    // This function is called after installing snapshot from leader
    virtual int reset(const int64_t next_log_index) = 0;

    // Create an instance of this kind of LogStorage with the parameters encoded 
    // in |uri|
    // Return the address referenced to the instance on success, NULL otherwise.
    virtual LogStorage* new_instance(const std::string& uri) const = 0;

    static LogStorage* create(const std::string& uri);
};

class RaftMetaStorage {
public:
    virtual ~RaftMetaStorage() {}

    // init stable storage, check consistency and integrity
    virtual int init() = 0;

    // set current term
    virtual int set_term(const int64_t term) = 0;

    // get current term
    virtual int64_t get_term() = 0;

    // set votefor information
    virtual int set_votedfor(const PeerId& peer_id) = 0;

    // get votefor information
    virtual int get_votedfor(PeerId* peer_id) = 0;

    // set term and votedfor information
    virtual int set_term_and_votedfor(const int64_t term, const PeerId& peer_id) = 0;

    // Create an instance of this kind of LogStorage with the parameters encoded 
    // in |uri|
    // Return the address referenced to the instance on success, NULL otherwise.
    virtual RaftMetaStorage* new_instance(const std::string& uri) const = 0;

    static RaftMetaStorage* create(const std::string& uri);
};

inline brpc::Extension<const LogStorage>* log_storage_extension() {
    return brpc::Extension<const LogStorage>::instance();
}

inline brpc::Extension<const RaftMetaStorage>* meta_storage_extension() {
    return brpc::Extension<const RaftMetaStorage>::instance();
}

#endif //~BRAFT_RAFT_STORAGE_H
