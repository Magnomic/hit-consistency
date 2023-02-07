#include <iostream>
#include <memory>
#include <string>
#include "state_machine.pb.h"
#include "cpp/node.h"
#include "cpp/raft.h"
#include "cpp/configuration.h"

#include <ctime>

#include <butil/logging.h>
#include <brpc/server.h>
#include <gflags/gflags.h>


DEFINE_string(server_addr, "0.0.0.0:8000", "Server listen address, may be IPV4/IPV6/UDS."
            " If this is set, the flag port will be ignored");
DEFINE_int32(block_size, 1 * 32u, "Size of block");
DEFINE_int32(request_size, 1, "Size of each requst");
DEFINE_int32(thread_num, 1, "thread number");
DEFINE_int32(dependency_percentage, 10, "dependency_percentage");

bvar::LatencyRecorder g_latency_recorder("block_client");

using hit_consistency::RaftService_Stub;
using hit_consistency::StateMachineService_Stub;
using hit_consistency::StateMachineRequest;
using hit_consistency::StateMachineResponse;


static void* sender(void* arg){
    
    std::string leader_addr(FLAGS_server_addr);

    brpc::Channel channel;
    if (channel.Init(leader_addr.c_str(), NULL) != 0) {
        LOG(ERROR) << "Fail to init channel to " << leader_addr;
            bthread_usleep(1 * 1000L);
    }
    int count(0);
    for (;;count++){
        // bthread_usleep(rand()%10);
        hit_consistency::StateMachineService_Stub stub(&channel);
        brpc::Controller cntl;
        cntl.set_timeout_ms(500);
        hit_consistency::StateMachineRequest request;
        hit_consistency::StateMachineResponse response;
        uint64_t offset = butil::fast_rand_less_than(
                            FLAGS_dependency_percentage);
        request.set_offset(offset);

        cntl.request_attachment().resize(FLAGS_request_size, (char) (rand() % 26 + 65));
        stub.write(&cntl, &request, &response, NULL);

        if (cntl.Failed()) {
            // LOG(WARNING) << "Fail to send request to " << leader_addr
            //             << " : " << cntl.ErrorText();
        }
        if (!response.success()) {
            // LOG(WARNING) << "Fail to send request to " << leader_addr
            //             << ", redirecting to "
            //             << (response.has_redirect() 
            //                     ? response.redirect() : "nowhere");
        }
        g_latency_recorder << cntl.latency_us();
    }
}

int main(int argc, char** argv) {

    gflags::ParseCommandLineFlags(&argc, &argv, true);

    std::string log_path = "test_result.log";

    ::logging::LoggingSettings log_setting; 
    log_setting.log_file = log_path.c_str(); 
    log_setting.logging_dest = logging::LOG_TO_FILE; 
    ::logging::InitLogging(log_setting);  


    butil::AtExitManager exit_manager;


    std::vector<bthread_t> tids;
    tids.resize(FLAGS_thread_num);

    for (int i = 0; i < FLAGS_thread_num; ++i) {
        if (bthread_start_background(&tids[i], NULL, sender, NULL) != 0) {
            LOG(ERROR) << "Fail to create bthread";
            return -1;
        }
    }

    std::vector<int64_t> qps_vector;
    std::vector<int64_t> latency_vector;
    int64_t fail_times = 0;
    int64_t num = 0;
    while (!brpc::IsAskedToQuit()) {
        sleep(1);
        int64_t qps = 0L;
        int64_t latency = 0L;
        // LOG(INFO)
        //         << " at qps=" << g_latency_recorder.qps(1)
        //         << " latency=" << g_latency_recorder.latency(1);
        if (g_latency_recorder.latency(1) < 490000){
            fail_times = 0;
            num++;
            // LOG(INFO) << num;
            qps_vector.insert(qps_vector.end(), g_latency_recorder.qps(1));
            latency_vector.insert(latency_vector.end(), g_latency_recorder.latency(1));
            sort(qps_vector.begin(), qps_vector.end());
            sort(latency_vector.begin(), latency_vector.end());
            if (num > 30) {
                int64_t times = 0;
                for (int i=qps_vector.size()/3; i<qps_vector.size()/3*2; i++){
                    qps += qps_vector[i];
                    latency += latency_vector[i];
                    times++;
                }
                LOG(INFO) << "thread_num = " << FLAGS_thread_num
                        << " qps = " << qps / times
                        << " latency = " << latency/ times;
                return 0;
            }
        } else {
            fail_times += 1;
        }
        if (fail_times > 5) {
            return -1;
        }
    }

    return 0;
}