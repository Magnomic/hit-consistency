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


DEFINE_string(server_addr, "0.0.0.0:8000", "Server listen address, may be IPV4/IPV6/UDS."
            " If this is set, the flag port will be ignored");

using hit_consistency::RaftService_Stub;
using hit_consistency::ClientRequest;
using hit_consistency::ClientResponse;


int main(int argc, char** argv) {

    google::ParseCommandLineFlags(&argc, &argv, true);

    std::string leader_addr(FLAGS_server_addr);

    brpc::Channel channel;
    if (channel.Init(leader_addr.c_str(), NULL) != 0) {
        LOG(ERROR) << "Fail to init channel to " << leader_addr;
            bthread_usleep(1 * 1000L);
    }
    int count(0);
    for (;;count++){
        hit_consistency::RaftService_Stub stub(&channel);
        brpc::Controller cntl;
        cntl.set_timeout_ms(500);
        hit_consistency::ClientResponse response;
        hit_consistency::ClientRequest request;
        request.set_payload(std::to_string(count));

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
    }
    return 0;
}