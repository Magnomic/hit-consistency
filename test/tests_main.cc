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
    ::testing::AddGlobalTestEnvironment(new TestEnvironment());
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    return RUN_ALL_TESTS();
}
