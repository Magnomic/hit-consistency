
#ifndef BRAFT_RAFT_CONFIGURATION_H
#define BRAFT_RAFT_CONFIGURATION_H

#include <string>
#include <ostream>
#include <vector>
#include <set>
#include <map>
#include <butil/strings/string_piece.h>
#include <butil/endpoint.h>
#include <butil/logging.h>

typedef std::string GroupId;

// Represent a participant in a replicating group.
struct PeerId {
    butil::EndPoint addr; // ip+port.
    int idx; // idx in same addr, default 0

    PeerId() : idx(0) {}
    explicit PeerId(butil::EndPoint addr_) : addr(addr_), idx(0) {}
    PeerId(butil::EndPoint addr_, int idx_) : addr(addr_), idx(idx_) {}
    /*intended implicit*/PeerId(const std::string& str) 
    { CHECK_EQ(0, parse(str)); }
    PeerId(const PeerId& id) : addr(id.addr), idx(id.idx) {}

    void reset() {
        addr.ip = butil::IP_ANY;
        addr.port = 0;
        idx = 0;
    }

    bool is_empty() const {
        return (addr.ip == butil::IP_ANY && addr.port == 0 && idx == 0);
    }

    int parse(const std::string& str) {
        reset();
        char ip_str[64];
        if (2 > sscanf(str.c_str(), "%[^:]%*[:]%d%*[:]%d", ip_str, &addr.port, &idx)) {
            reset();
            return -1;
        }
        if (0 != butil::str2ip(ip_str, &addr.ip)) {
            reset();
            return -1;
        }
        return 0;
    }

    std::string to_string() const {
        char str[128];
        snprintf(str, sizeof(str), "%s:%d", butil::endpoint2str(addr).c_str(), idx);
        return std::string(str);
    }
};

inline bool operator<(const PeerId& id1, const PeerId& id2) {
    if (id1.addr < id2.addr) {
        return true;
    } else {
        return id1.addr == id2.addr && id1.idx < id2.idx;
    }
}

inline bool operator==(const PeerId& id1, const PeerId& id2) {
    return (id1.addr == id2.addr && id1.idx == id2.idx);
}

inline bool operator!=(const PeerId& id1, const PeerId& id2) {
    return (id1.addr != id2.addr || id1.idx != id2.idx);
}

inline std::ostream& operator << (std::ostream& os, const PeerId& id) {
    return os << id.addr << ':' << id.idx;
}

struct NodeId {
    GroupId group_id;
    PeerId peer_id;

    NodeId(const GroupId& group_id_, const PeerId& peer_id_)
        : group_id(group_id_), peer_id(peer_id_) {
    }
    std::string to_string() const;
};

inline bool operator<(const NodeId& id1, const NodeId& id2) {
    const int rc = id1.group_id.compare(id2.group_id);
    if (rc < 0) {
        return true;
    } else {
        return rc == 0 && id1.peer_id < id2.peer_id;
    }
}

inline bool operator==(const NodeId& id1, const NodeId& id2) {
    return (id1.group_id == id2.group_id && id1.peer_id == id2.peer_id);
}

inline bool operator!=(const NodeId& id1, const NodeId& id2) {
    return (id1.group_id != id2.group_id || id1.peer_id != id2.peer_id);
}

inline std::ostream& operator << (std::ostream& os, const NodeId& id) {
    return os << id.group_id << ':' << id.peer_id;
}

inline std::string NodeId::to_string() const {
    std::ostringstream oss;
    oss << *this;
    return oss.str();
}
#endif