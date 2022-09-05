#include "configuration.h"
#include <butil/logging.h>
#include <butil/string_splitter.h>

std::ostream& operator<<(std::ostream& os, const Configuration& a) {
    std::vector<PeerId> peers;
    a.list_peers(&peers);
    for (size_t i = 0; i < peers.size(); i++) {
        os << peers[i];
        if (i < peers.size() - 1) {
            os << ",";
        }
    }
    return os;
}

int Configuration::parse_from(butil::StringPiece conf) {
    reset();
    std::string peer_str;
    for (butil::StringSplitter sp(conf.begin(), conf.end(), ','); sp; ++sp) {
        PeerId peer;
        peer_str.assign(sp.field(), sp.length());
        if (peer.parse(peer_str) != 0) {
            LOG(ERROR) << "Fail to parse " << peer_str;
            return -1;
        }
        add_peer(peer);
    }
    return 0;
}
