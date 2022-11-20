#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <butil/logging.h>
#include <butil/files/file_path.h>
#include <butil/file_util.h>
#include <butil/fast_rand.h>
#include <brpc/closure_guard.h>
#include <bthread/bthread.h>
#include <bthread/countdown_event.h>
#include "cpp/macros.h"

using raft::raft_mutex_t;
class TestEnvironment : public ::testing::Environment {
public:
    void SetUp() {
    }
    void TearDown() {
    }
};

int main(int argc, char* argv[]) {
    std::vector<int64_t> entries = {1 ,2 ,3 ,4 ,5};
    std::cout << " entries->begin() : " << *entries.begin() << std::endl;
    std::cout << " entries->end() : " << *(entries.end()-1) << std::endl;
    entries.erase(entries.begin(), entries.end());
    std::cout<< entries.size() << std::endl;
    for (int64_t i =0;i<5;++i){
        std::cout<< i << std::endl;
    }
}
