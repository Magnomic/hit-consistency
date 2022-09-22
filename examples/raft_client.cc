#include <iostream>
#include <memory>
#include <string>
#include "state_machine.pb.h"
#include "cpp/node.h"
#include "cpp/raft.h"
#include "cpp/configuration.h"

#include <butil/logging.h>
#include <brpc/server.h>
#include <gflags/gflags.h>


DEFINE_string(server_addr, "0.0.0.0:8000", "Server listen address, may be IPV4/IPV6/UDS."
            " If this is set, the flag port will be ignored");
DEFINE_int32(block_size, 64 * 1024u, "Size of block");
DEFINE_int32(request_size, 1024, "Size of each requst");

using hit_consistency::RaftService_Stub;
using hit_consistency::StateMachineService_Stub;
using hit_consistency::StateMachineRequest;
using hit_consistency::StateMachineResponse;


int main(int argc, char** argv) {

    google::ParseCommandLineFlags(&argc, &argv, true);

    std::string leader_addr(FLAGS_server_addr);

    brpc::Channel channel;
    if (channel.Init(leader_addr.c_str(), NULL) != 0) {
        LOG(ERROR) << "Fail to init channel to " << leader_addr;
            bthread_usleep(1 * 1000L);
    }
    int count(0);
    for (;count < 1;count++){
        hit_consistency::StateMachineService_Stub stub(&channel);
        brpc::Controller cntl;
        cntl.set_timeout_ms(500);
        hit_consistency::StateMachineRequest request;
        hit_consistency::StateMachineResponse response;
        request.set_offset(butil::fast_rand_less_than(
                            FLAGS_block_size - FLAGS_request_size));

        cntl.request_attachment().resize(FLAGS_request_size, 'a');
        stub.write(&cntl, &request, &response, NULL);

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