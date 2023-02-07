#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <bitset>
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
    std::bitset<1024> bitmap(0);
    int64_t num = (1L << 63) - 1;
    bitmap.set(1023);
    bitmap.set(1);
    bitmap.set(62);
    std::cout<< "bitmap >> 1 = " << (bitmap>>1) << std::endl;
    std::cout<< "bitmap & int64_t = " << (bitmap & std::bitset<1024>(num)) << std::endl;
    int64_t temp = (1L << 63) - 1;
    std::cout << std::bitset<64>(temp) << std::endl;
    std::cout << std::bitset<64>(-1)<< std::endl;
    std::cout << std::bitset<64>(-1 >> 1)<< std::endl;
    std::cout << std::bitset<64>(-1 << 1)<< std::endl;
    std::cout << " entries->begin() : " << *entries.begin() << std::endl;
    std::cout << " entries->end() : " << *(entries.end()-1) << std::endl;
    entries.erase(entries.begin(), entries.end());
    std::cout<< entries.size() << std::endl;
    for (int64_t i =0;i<5;++i){
        std::cout<< i << std::endl;
    }
}
