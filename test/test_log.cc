#include <gtest/gtest.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <butil/atomicops.h>
#include <butil/file_util.h>
#include <butil/files/file_path.h>
#include <butil/files/file_enumerator.h>
#include <butil/files/dir_reader_posix.h>
#include <butil/string_printf.h>
#include <butil/logging.h>
#include "cpp/util.h"
#include "cpp/log.h"
#include "cpp/storage.h"

class LogStorageTest : public testing::Test {
protected:
    void SetUp() {
        FLAGS_raft_sync = false;
        gflags::SetCommandLineOption("minloglevel", "3");
    }
    void TearDown() {}
};

TEST_F(LogStorageTest, open_segment) {
    // open segment operation
    ::system("mkdir data/");
    Segment* seg1 = new Segment("./data", 1L, 0);

    // not open
    LogEntry* entry = seg1->get(1);
    ASSERT_TRUE(entry == NULL);

    // create and open
    ASSERT_EQ(0, seg1->create());
    ASSERT_TRUE(seg1->is_open());

    std::cout << "goes here" << std::endl;
    // append entry
    for (int i = 0; i < 10; i++) {
        LogEntry* entry = new LogEntry();
        entry->AddRef();
        entry->type = hit_consistency::ENTRY_TYPE_DATA;
        entry->id.term = 1;
        entry->id.index = i + 1;

        char data_buf[128];
        snprintf(data_buf, sizeof(data_buf), "hello, world: %d", i + 1);
        entry->data.append(data_buf);

        ASSERT_EQ(0, seg1->append(entry));

        entry->Release();
    }

    // read entry
    for (int i = 0; i < 10; i++) {
        int64_t term = seg1->get_term(i+1);
        ASSERT_EQ(term, 1);

        LogEntry* entry = seg1->get(i+1);
        ASSERT_EQ(entry->id.term, 1);
        ASSERT_EQ(entry->type, hit_consistency::ENTRY_TYPE_DATA);
        ASSERT_EQ(entry->id.index, i+1);

        char data_buf[128];
        snprintf(data_buf, sizeof(data_buf), "hello, world: %d", i + 1);
        ASSERT_EQ(data_buf, entry->data.to_string());
        entry->Release();
    }
    {
        LogEntry* entry = seg1->get(0);
        ASSERT_TRUE(entry == NULL);
        entry = seg1->get(11);
        ASSERT_TRUE(entry == NULL);
    }
}