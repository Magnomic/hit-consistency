#ifndef  RAFT_LOG_ENTRY_H
#define  RAFT_LOG_ENTRY_H

#include <butil/iobuf.h>                         // butil::IOBuf
#include <butil/status.h>                         // butil::IOBuf
#include <butil/memory/ref_counted.h>            // butil::RefCountedThreadSafe
#include <butil/third_party/murmurhash3/murmurhash3.h>  // fmix64
#include <bvar/bvar.h>
#include "configuration.h"
#include "raft_message.pb.h"
#include "local_storage.pb.h"
#include "enum.pb.h"

using hit_consistency::ENTRY_TYPE_UNKNOWN;
using hit_consistency::ConfigurationPBMeta;

struct LogId {
    LogId() : index(0), term(0) {}
    LogId(int64_t index_, int64_t term_) : index(index_), term(term_) {}
    int64_t index;
    int64_t term;
};

// term start from 1, log index start from 1
struct LogEntry : public butil::RefCountedThreadSafe<LogEntry> {
public:
    hit_consistency::EntryType type; // log type
    LogId id;
    int64_t dependency;
    std::vector<PeerId>* peers; // peers
    std::vector<PeerId>* old_peers; // peers
    butil::IOBuf data;

    LogEntry();

    virtual ~LogEntry();
private:
    DISALLOW_COPY_AND_ASSIGN(LogEntry);
    friend class butil::RefCountedThreadSafe<LogEntry>;
};

// Comparators

inline bool operator==(const LogId& lhs, const LogId& rhs) {
    return lhs.index == rhs.index && lhs.term == rhs.term;
}

inline bool operator!=(const LogId& lhs, const LogId& rhs) {
    return !(lhs == rhs);
}

inline bool operator<(const LogId& lhs, const LogId& rhs) {
    if (lhs.term == rhs.term) {
        return lhs.index < rhs.index;
    }
    return lhs.term < rhs.term;
}

inline bool operator>(const LogId& lhs, const LogId& rhs) {
    if (lhs.term == rhs.term) {
        return lhs.index > rhs.index;
    }
    return lhs.term > rhs.term;
}

inline bool operator<=(const LogId& lhs, const LogId& rhs) {
    return !(lhs > rhs);
}

inline bool operator>=(const LogId& lhs, const LogId& rhs) {
    return !(lhs < rhs);
}

struct LogIdHasher {
    size_t operator()(const LogId& id) const {
        return butil::fmix64(id.index) ^ butil::fmix64(id.term);
    }
};

inline std::ostream& operator<<(std::ostream& os, const LogId& id) {
    os << "(index=" << id.index << ",term=" << id.term << ')';
    return os;
}

butil::Status parse_configuration_meta(const butil::IOBuf& data, LogEntry* entry);

butil::Status serialize_configuration_meta(const LogEntry* entry, butil::IOBuf& data);

#endif  //BRAFT_LOG_ENTRY_H