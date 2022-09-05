#include "configuration_manager.h"


int ConfigurationManager::add(const ConfigurationEntry& entry) {
    if (!_configurations.empty()) {
        if (_configurations.back().id.index >= entry.id.index) {
            CHECK(false) << "Did you forget to call truncate_suffix before "
                            " the last log index goes back";
            return -1;
        }
    }
    _configurations.push_back(entry);
    return 0;
}

void ConfigurationManager::truncate_prefix(const int64_t first_index_kept) {
    while (!_configurations.empty()
            && _configurations.front().id.index < first_index_kept) {
        _configurations.pop_front();
    }
}

void ConfigurationManager::truncate_suffix(const int64_t last_index_kept) {
    while (!_configurations.empty()
        && _configurations.back().id.index > last_index_kept) {
        _configurations.pop_back();
    }
}


void ConfigurationManager::get(int64_t last_included_index,
                               ConfigurationEntry* conf) {
    if (_configurations.empty()) {
        CHECK_GE(last_included_index, _snapshot.id.index);
        *conf = _snapshot;
        return;
    }
    std::deque<ConfigurationEntry>::iterator it;
    for (it = _configurations.begin(); it != _configurations.end(); ++it) {
        if (it->id.index > last_included_index) {
            break;
        }
    }
    if (it == _configurations.begin()) {
        *conf = _snapshot;
        return;
    }
    --it;
    *conf = *it;
}

const ConfigurationEntry& ConfigurationManager::last_configuration() const {
    if (!_configurations.empty()) {
        return _configurations.back();
    }
    return _snapshot;
}
