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


DEFINE_string(conf, "0.0.0.0:8000:0,0.0.0.0:8001:0", "Initial configuration of the replication group");
DEFINE_int32(election_timeout_ms, 5000, 
            "Start election in such milliseconds if disconnect with the leader");
DEFINE_bool(echo_attachment, true, "Echo attachment as well");
DEFINE_string(data_path, "data", "Path of data stored on");
DEFINE_int32(port, 8000, "TCP Port of this server");
DEFINE_string(listen_addr, "0.0.0.0", "Server listen address, may be IPV4/IPV6/UDS."
            " If this is set, the flag port will be ignored");
DEFINE_int32(idle_timeout_s, -1, "Connection will be closed if there is no "
             "read/write operations during the last `idle_timeout_s'");
DEFINE_int32(logoff_ms, 2000, "Maximum duration of server's LOGOFF state "
             "(waiting for client to close connection before server stops)");
DEFINE_int32(snapshot_interval, 30, "Interval between each snapshot");
DEFINE_bool(disable_cli, false, "Don't allow raft_cli access this node");
DEFINE_string(group, "Atomic", "Id of the replication group");

using hit_consistency::RaftService_Stub;
using hit_consistency::ClientRequest;
using hit_consistency::ClientResponse;


int main(int argc, char** argv) {

    google::ParseCommandLineFlags(&argc, &argv, true);

    std::string leader_addr("0.0.0.0:8000");

    brpc::Channel channel;
    if (channel.Init(leader_addr, NULL) != 0) {
        LOG(ERROR) << "Fail to init channel to " << leader_addr;
            bthread_usleep(1 * 1000L);
    }

    hit_consistency::RaftService_Stub stub(&channel);
    brpc::Controller cntl;
    cntl.set_timeout_ms(5000);
    // Randomly select which request we want send;
    hit_consistency::ClientResponse response;

    hit_consistency::ClientRequest request;
    request.set_value("1234567890");
    stub.client_request(&cntl, &request, &response, NULL);


    if (cntl.Failed()) {
        LOG(WARNING) << "Fail to send request to " << leader_addr
                      << " : " << cntl.ErrorText();
    }
    if (!response.success()) {
        LOG(WARNING) << "Fail to send request to " << leader_addr
                      << ", redirecting to "
                      << (response.has_redirect() 
                            ? response.redirect() : "nowhere");
    }
    return 0;
}