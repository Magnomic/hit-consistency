#ifndef BRAFT_PROTOBUF_FILE_H
#define BRAFT_PROTOBUF_FILE_H

#include <string>
#include <google/protobuf/message.h>
#include "file_system_adaptor.h"


// protobuf file format:
// len [4B, in network order]
// protobuf data
class ProtoBufFile {
public:
    ProtoBufFile(const char* path, FileSystemAdaptor* fs = NULL);
    ProtoBufFile(const std::string& path, FileSystemAdaptor* fs = NULL);
    ~ProtoBufFile() {}

    int save(const ::google::protobuf::Message* message, bool sync);
    int load(::google::protobuf::Message* message);

private:
    std::string _path;
    scoped_refptr<FileSystemAdaptor> _fs;
};


#endif //~BRAFT_PROTOBUF_FILE_H