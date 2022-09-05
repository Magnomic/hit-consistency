#ifndef  BRAFT_CONFIGURATION_MANAGER_H
#define  BRAFT_CONFIGURATION_MANAGER_H

#include "configuration.h"         // Configuration
#include "log_entry.h"             // LogId


struct ConfigurationEntry {
    LogId id;
    Configuration conf;
    Configuration old_conf;

    ConfigurationEntry() {}
    ConfigurationEntry(const LogEntry& entry) {
        id = entry.id;
        conf = *(entry.peers);
        if (entry.old_peers) {
            old_conf = *(entry.old_peers);
        }
    }

    bool stable() const { return old_conf.empty(); }
    bool empty() const { return conf.empty(); }
    void list_peers(std::set<PeerId>* peers) {
        peers->clear();
        conf.append_peers(peers);
        old_conf.append_peers(peers);
    }
    bool contains(const PeerId& peer) const
    { return conf.contains(peer) || old_conf.contains(peer); }
};


// Manager the history of configuration changing
class ConfigurationManager {
public:
    ConfigurationManager() {}
    ~ConfigurationManager() {}

    // add new configuration at index
    int add(const ConfigurationEntry& entry);

    // [1, first_index_kept) are being discarded
    void truncate_prefix(int64_t first_index_kept);

    // (last_index_kept, infinity) are being discarded
    void truncate_suffix(int64_t last_index_kept);

    void get(int64_t last_included_index, ConfigurationEntry* entry);

    const ConfigurationEntry& last_configuration() const;

private:

    std::deque<ConfigurationEntry> _configurations;
    ConfigurationEntry _snapshot;
};

#endif  //BRAFT_CONFIGURATION_MANAGER_H