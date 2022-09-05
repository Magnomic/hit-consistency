#include "log_entry.h"
#include "local_storage.pb.h"


bvar::Adder<int64_t> g_nentries("raft_num_log_entries");

LogEntry::LogEntry(): type(ENTRY_TYPE_UNKNOWN), peers(NULL), old_peers(NULL) {
    g_nentries << 1;
}

LogEntry::~LogEntry() {
    g_nentries << -1;
    delete peers;
    delete old_peers;
}

butil::Status parse_configuration_meta(const butil::IOBuf& data, LogEntry* entry) {
    butil::Status status;
    ConfigurationPBMeta meta;
    butil::IOBufAsZeroCopyInputStream wrapper(data);
    if (!meta.ParseFromZeroCopyStream(&wrapper)) {
        status.set_error(EINVAL, "Fail to parse ConfigurationPBMeta");
        return status;
    }
    entry->peers = new std::vector<PeerId>;
    for (int j = 0; j < meta.peers_size(); ++j) {
        entry->peers->push_back(PeerId(meta.peers(j)));
    }
    if (meta.old_peers_size() > 0) {
        entry->old_peers = new std::vector<PeerId>;
        for (int i = 0; i < meta.old_peers_size(); i++) {
            entry->old_peers->push_back(PeerId(meta.old_peers(i)));
        }
    }
    return status;    
}

butil::Status serialize_configuration_meta(const LogEntry* entry, butil::IOBuf& data) {
    butil::Status status;
    ConfigurationPBMeta meta;
    for (size_t i = 0; i < entry->peers->size(); ++i) {
        meta.add_peers((*(entry->peers))[i].to_string());
    }
    if (entry->old_peers) {
        for (size_t i = 0; i < entry->old_peers->size(); ++i) {
            meta.add_old_peers((*(entry->old_peers))[i].to_string());
        }
    }
    butil::IOBufAsZeroCopyOutputStream wrapper(&data);
    if (!meta.SerializeToZeroCopyStream(&wrapper)) {
        status.set_error(EINVAL, "Fail to serialize ConfigurationPBMeta");
    }
    return status;
}