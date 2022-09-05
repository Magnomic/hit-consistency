#include <errno.h>
#include <butil/time.h>
#include <butil/logging.h>
#include <butil/file_util.h>                         // butil::CreateDirectory
#include "util.h"
#include "protobuf_file.h"
#include "local_storage.pb.h"
#include "raft_meta.h"

using hit_consistency::StablePBMeta;

const char* LocalRaftMetaStorage::_s_raft_meta = "raft_meta";

int LocalRaftMetaStorage::init() {
    if (_is_inited) {
        return 0;
    }
    butil::FilePath dir_path(_path);
    butil::File::Error e;
    if (!butil::CreateDirectoryAndGetError(
                dir_path, &e, FLAGS_raft_create_parent_directories)) {
        LOG(ERROR) << "Fail to create " << dir_path.value() << " : " << e;
        return -1;
    }

    int ret = load();
    if (ret == 0) {
        _is_inited = true;
    }
    return ret;
}

int LocalRaftMetaStorage::set_term(const int64_t term) {
    if (_is_inited) {
        _term = term;
        return save();
    } else {
        LOG(WARNING) << "LocalRaftMetaStorage not init(), path: " << _path;
        return -1;
    }
}

int64_t LocalRaftMetaStorage::get_term() {
    if (_is_inited) {
        return _term;
    } else {
        LOG(WARNING) << "LocalRaftMetaStorage not init(), path: " << _path;
        return -1;
    }
}

int LocalRaftMetaStorage::set_votedfor(const PeerId& peer_id) {
    if (_is_inited) {
        _votedfor = peer_id;
        return save();
    } else {
        LOG(WARNING) << "LocalRaftMetaStorage not init(), path: " << _path;
        return -1;
    }
}

int LocalRaftMetaStorage::set_term_and_votedfor(const int64_t term, const PeerId& peer_id) {
    if (_is_inited) {
        _term = term;
        _votedfor = peer_id;
        return save();
    } else {
        LOG(WARNING) << "LocalRaftMetaStorage not init(), path: " << _path;
        return -1;
    }
}

int LocalRaftMetaStorage::load() {

    std::string path(_path);
    path.append("/");
    path.append(_s_raft_meta);

    ProtoBufFile pb_file(path);

    StablePBMeta meta;
    int ret = pb_file.load(&meta);
    if (ret == 0) {
        _term = meta.term();
        ret = _votedfor.parse(meta.votedfor());
    } else if (errno == ENOENT) {
        ret = 0;
    } else {
        PLOG(ERROR) << "Fail to load meta from " << path;
    }

    return ret;
}

int LocalRaftMetaStorage::save() {
    butil::Timer timer;
    timer.start();

    StablePBMeta meta;
    meta.set_term(_term);
    meta.set_votedfor(_votedfor.to_string());

    std::string path(_path);
    path.append("/");
    path.append(_s_raft_meta);

    ProtoBufFile pb_file(path);
    int ret = pb_file.save(&meta, raft_sync_meta());
    PLOG_IF(ERROR, ret != 0) << "Fail to save meta to " << path;

    timer.stop();
    LOG(INFO) << "save raft meta, path " << _path
        << " term " << _term << " votedfor " << _votedfor.to_string() << " time: " << timer.u_elapsed();
    return ret;
}

int LocalRaftMetaStorage::get_votedfor(PeerId* peer_id) {
    if (_is_inited) {
        *peer_id = _votedfor;
        return 0;
    } else {
        LOG(WARNING) << "LocalRaftMetaStorage not init(), path: " << _path;
        return -1;
    }
}

RaftMetaStorage* LocalRaftMetaStorage::new_instance(const std::string& uri) const {
    return new LocalRaftMetaStorage(uri);
}

