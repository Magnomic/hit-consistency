#include <iostream>
#include <memory>
#include <string>
#include "raft_message.pb.h"
#include "cpp/node.h"

#include <butil/logging.h>
#include <brpc/server.h>
#include <gflags/gflags.h>


DEFINE_bool(echo_attachment, true, "Echo attachment as well");
DEFINE_int32(port, 8000, "TCP Port of this server");
DEFINE_string(listen_addr, "", "Server listen address, may be IPV4/IPV6/UDS."
            " If this is set, the flag port will be ignored");
DEFINE_int32(idle_timeout_s, -1, "Connection will be closed if there is no "
             "read/write operations during the last `idle_timeout_s'");
DEFINE_int32(logoff_ms, 2000, "Maximum duration of server's LOGOFF state "
             "(waiting for client to close connection before server stops)");

int main(int argc, char** argv) {

    google::ParseCommandLineFlags(&argc, &argv, true);

    NodeImpl& node = NodeImpl::getInstance();

    std::string server_addr(FLAGS_listen_addr.c_str());
    int server_port(FLAGS_port);
    int server_timeout(FLAGS_idle_timeout_s);
    
    node.init(server_addr, server_port, server_timeout);

    node.start();
 
    return 0;
}