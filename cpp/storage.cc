#include <errno.h>
#include <butil/string_printf.h>
#include <butil/string_splitter.h>
#include <butil/strings/string_piece.h>
#include <butil/logging.h>
#include <brpc/reloadable_flags.h>

#include "storage.h"
#include "log.h"

DEFINE_bool(raft_sync, true, "call fsync when need");
BRPC_VALIDATE_GFLAG(raft_sync, ::brpc::PassValidate);
DEFINE_bool(raft_create_parent_directories, true,
            "Create parent directories of the path in local storage if true");

DEFINE_bool(raft_sync_meta, false, "sync log meta, snapshot meta and raft meta");
BRPC_VALIDATE_GFLAG(raft_sync_meta, ::brpc::PassValidate);

inline butil::StringPiece parse_uri(butil::StringPiece* uri, std::string* parameter) {
    // ${protocol}://${parameters}
    size_t pos = uri->find("://");
    if (pos == butil::StringPiece::npos) {
        return butil::StringPiece();
    }
    butil::StringPiece protocol = uri->substr(0, pos);
    uri->remove_prefix(pos + 3/* length of '://' */);
    protocol.trim_spaces();
    parameter->reserve(uri->size());
    parameter->clear();
    size_t removed_spaces = 0;
    for (butil::StringPiece::const_iterator 
            iter = uri->begin(); iter != uri->end(); ++iter) {
        if (!isspace(*iter)) {
            parameter->push_back(*iter);
        } else {
            ++removed_spaces;
        }
    }
    LOG_IF(WARNING, removed_spaces) << "Removed " << removed_spaces 
            << " spaces from `" << *uri << '\'';
    return protocol;
}

LogStorage* LogStorage::create(const std::string& uri) {
    butil::StringPiece copied_uri(uri);
    std::string parameter;
    butil::StringPiece protocol = parse_uri(&copied_uri, &parameter);
    if (protocol.empty()) {
        LOG(ERROR) << "Invalid log storage uri=`" << uri << '\'';
        return NULL;
    }
    const LogStorage* type = log_storage_extension()->Find(
                protocol.as_string().c_str());
    if (type == NULL) {
        LOG(ERROR) << "Fail to find log storage type " << protocol
                   << ", uri=" << uri;
        return NULL;
    }
    return type->new_instance(parameter);
}

RaftMetaStorage* RaftMetaStorage::create(const std::string& uri) {
    butil::StringPiece copied_uri(uri);
    std::string parameter;
    butil::StringPiece protocol = parse_uri(&copied_uri, &parameter);
    if (protocol.empty()) {
        LOG(ERROR) << "Invalid meta storage uri=`" << uri << '\'';
        return NULL;
    }
    const RaftMetaStorage* type = meta_storage_extension()->Find(
                protocol.as_string().c_str());
    if (type == NULL) {
        LOG(ERROR) << "Fail to find meta storage type " << protocol
                   << ", uri=" << uri;
        return NULL;
    }
    return type->new_instance(parameter);
}