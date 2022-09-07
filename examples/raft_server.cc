#include <iostream>
#include <memory>
#include <string>
#include "raft_message.pb.h"
#include "cpp/node.h"
#include "cpp/raft.h"
#include "cpp/configuration.h"

#include <butil/logging.h>
#include <brpc/server.h>
#include <gflags/gflags.h>


DEFINE_string(conf, "127.0.0.1:8000:0,127.0.0.1:8001:0", "Initial configuration of the replication group");
DEFINE_int32(election_timeout_ms, 5000, 
            "Start election in such milliseconds if disconnect with the leader");
DEFINE_bool(echo_attachment, true, "Echo attachment as well");
DEFINE_string(data_path, "./data", "Path of data stored on");
DEFINE_int32(port, 8000, "TCP Port of this server");
DEFINE_string(listen_addr, "0.0.0.0", "Server listen address, may be IPV4/IPV6/UDS."
            " If this is set, the flag port will be ignored");
DEFINE_int32(idle_timeout_s, -1, "Connection will be closed if there is no "
             "read/write operations during the last `idle_timeout_s'");
DEFINE_int32(logoff_ms, 2000, "Maximum duration of server's LOGOFF state "
             "(waiting for client to close connection before server stops)");
DEFINE_int32(snapshot_interval, 30, "Interval between each snapshot");
DEFINE_bool(disable_cli, false, "Don't allow raft_cli access this node");


int main(int argc, char** argv) {

    google::ParseCommandLineFlags(&argc, &argv, true);

    NodeImpl& node = NodeImpl::getInstance();

    std::string server_addr(FLAGS_listen_addr.c_str());
    
    butil::EndPoint addr(butil::IP_ANY, FLAGS_port);

    NodeOptions node_options;
    if (node_options.initial_conf.parse_from(FLAGS_conf) != 0) {
        LOG(ERROR) << "Fail to parse configuration `" << FLAGS_conf << '\'';
        return -1;
    }
    node_options.election_timeout_ms = FLAGS_election_timeout_ms;
    node_options.node_owns_fsm = false;
    node_options.snapshot_interval_s = FLAGS_snapshot_interval;
    std::string prefix = "local://" + FLAGS_data_path;
    node_options.log_uri = prefix + "/log";
    node_options.raft_meta_uri = prefix + "/raft_meta";
    node_options.snapshot_uri = prefix + "/snapshot";
    node_options.disable_cli = FLAGS_disable_cli;

    node.init(node_options, PeerId(addr));

    node.start();
 
    return 0;
}