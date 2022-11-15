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

using hit_consistency::ENTRY_TYPE_NO_OP;
using hit_consistency::ENTRY_TYPE_DATA;
using hit_consistency::ENTRY_TYPE_CONFIGURATION;

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

    // append entry
    for (int i = 9; i >= 0; i--) {
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

    ConfigurationManager* configuration_manager = new ConfigurationManager;
    // load open segment
    Segment* seg2 = new Segment("./data", 1, 0);
    ASSERT_EQ(0, seg2->load(configuration_manager));

    for (int i = 0; i < 10; i++) {
        LogEntry* entry = seg2->get(i+1);
        ASSERT_EQ(entry->id.term, 1);
        ASSERT_EQ(entry->type, ENTRY_TYPE_DATA);
        ASSERT_EQ(entry->id.index, i+1);

        char data_buf[128];
        snprintf(data_buf, sizeof(data_buf), "hello, world: %d", i + 1);
        ASSERT_EQ(data_buf, entry->data.to_string());
        entry->Release();
    }
    {
        LogEntry* entry = seg2->get(0);
        ASSERT_TRUE(entry == NULL);
        entry = seg2->get(11);
        ASSERT_TRUE(entry == NULL);
    }
    // delete seg2;

    // truncate and read
    ASSERT_EQ(0, seg1->truncate(5));
    for (int i = 4; i >=0; i--) {
        LogEntry* entry = new LogEntry();
        entry->type = ENTRY_TYPE_DATA;
        entry->id.term = 1;
        entry->id.index = i + 6;

        char data_buf[128];
        snprintf(data_buf, sizeof(data_buf), "HELLO, WORLD: %d", i + 6);
        entry->data.append(data_buf); 

        ASSERT_EQ(0, seg1->append(entry));

        entry->Release();
    }
    for (int i = 0; i < 10; i++) {
        LogEntry* entry = seg1->get(i+1);
        ASSERT_EQ(entry->id.term, 1);
        ASSERT_EQ(entry->type, ENTRY_TYPE_DATA);
        ASSERT_EQ(entry->id.index, i+1);

        char data_buf[128];
        if (i < 5) {
            snprintf(data_buf, sizeof(data_buf), "hello, world: %d", i + 1);
        } else {
            snprintf(data_buf, sizeof(data_buf), "HELLO, WORLD: %d", i + 1);
        }
        ASSERT_EQ(data_buf, entry->data.to_string());
        entry->Release();
    }

    ASSERT_EQ(0, seg1->close());
    ASSERT_FALSE(seg1->is_open());
    ASSERT_EQ(0, seg1->unlink());

    delete configuration_manager;
}


TEST_F(LogStorageTest, closed_segment) {
    // open segment operation
    Segment* seg1 = new Segment("./data", 1L, 0);
    ASSERT_EQ(0, seg1->create());
    ASSERT_TRUE(seg1->is_open());
    // append entry
    for (int i = 0; i < 10; i++) {
        LogEntry* entry = new LogEntry();
        entry->type = ENTRY_TYPE_DATA;
        entry->id.term = 1;
        entry->id.index = i + 1;

        char data_buf[128];
        snprintf(data_buf, sizeof(data_buf), "hello, world: %d", i + 1);
        entry->data.append(data_buf);

        ASSERT_EQ(0, seg1->append(entry));

        entry->Release();
    }
    seg1->close();

    // read entry
    for (int i = 0; i < 10; i++) {
        LogEntry* entry = seg1->get(i+1);
        ASSERT_EQ(entry->id.term, 1);
        ASSERT_EQ(entry->type, ENTRY_TYPE_DATA);
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

    ConfigurationManager* configuration_manager = new ConfigurationManager;
    // load open segment
    Segment* seg2 = new Segment("./data", 1, 10, 0);
    ASSERT_EQ(0, seg2->load(configuration_manager));

    for (int i = 0; i < 10; i++) {
        LogEntry* entry = seg2->get(i+1);
        ASSERT_EQ(entry->id.term, 1);
        ASSERT_EQ(entry->type, ENTRY_TYPE_DATA);
        ASSERT_EQ(entry->id.index, i+1);

        char data_buf[128];
        snprintf(data_buf, sizeof(data_buf), "hello, world: %d", i + 1);
        ASSERT_EQ(data_buf, entry->data.to_string());
        entry->Release();
    }
    {
        LogEntry* entry = seg2->get(0);
        ASSERT_TRUE(entry == NULL);
        entry = seg2->get(11);
        ASSERT_TRUE(entry == NULL);
    }

    // truncate and read
    ASSERT_EQ(0, seg1->truncate(5));
    for (int i = 0; i < 5; i++) {
        LogEntry* entry = new LogEntry();
        entry->type = ENTRY_TYPE_DATA;
        entry->id.term = 1;
        entry->id.index = i + 6;

        char data_buf[128];
        snprintf(data_buf, sizeof(data_buf), "HELLO, WORLD: %d", i + 6);
        entry->data.append(data_buf);

        // become open segment again
        ASSERT_EQ(0, seg1->append(entry));

        entry->Release();
    }
    for (int i = 0; i < 10; i++) {
        LogEntry* entry = seg1->get(i+1);
        char data_buf[128];
        if (i < 5) {
            snprintf(data_buf, sizeof(data_buf), "hello, world: %d", i + 1);
        } else {
            snprintf(data_buf, sizeof(data_buf), "HELLO, WORLD: %d", i + 1);
        }
        ASSERT_EQ(entry->id.term, 1);
        ASSERT_EQ(entry->type, ENTRY_TYPE_DATA);
        ASSERT_EQ(entry->id.index, i+1);
        ASSERT_EQ(data_buf, entry->data.to_string());
        entry->Release();
    }

    ASSERT_EQ(0, seg1->unlink());

    delete configuration_manager;
}

TEST_F(LogStorageTest, multi_segment_and_segment_logstorage) {
    ::system("rm -rf data");
    SegmentLogStorage* storage = new SegmentLogStorage("./data");

    // init
    ASSERT_EQ(0, storage->init(new ConfigurationManager()));
    ASSERT_EQ(1, storage->first_log_index());
    ASSERT_EQ(0, storage->last_log_index());

    // append entry
    for (int i = 0; i < 100000; i++) {
        std::vector<LogEntry*> entries;
        for (int j = 4; j >= 0; j--) {
            int64_t index = 5*i + j + 1;
            LogEntry* entry = new LogEntry();
            entry->type = ENTRY_TYPE_DATA;
            entry->id.term = 1;
            entry->id.index = index;

            char data_buf[128];
            snprintf(data_buf, sizeof(data_buf), "hello, world: %" PRId64, index);
            entry->data.append(data_buf);
            entries.push_back(entry);
        }

        ASSERT_EQ(5, storage->append_entries(entries));

        for (size_t j = 0; j < entries.size(); j++) {
            entries[j]->Release();
        }
    }

    // read entry
    for (int i = 0; i < 500000; i++) {
        int64_t index = i + 1;
        LogEntry* entry = storage->get_entry(index);
        ASSERT_EQ(entry->id.term, 1);
        ASSERT_EQ(entry->type, ENTRY_TYPE_DATA);
        ASSERT_EQ(entry->id.index, index);

        char data_buf[128];
        snprintf(data_buf, sizeof(data_buf), "hello, world: %" PRId64, index);
        ASSERT_EQ(data_buf, entry->data.to_string());
        entry->Release();
    }

    ASSERT_EQ(storage->first_log_index(), 1);
    ASSERT_EQ(storage->last_log_index(), 500000);
    // truncate prefix
    ASSERT_EQ(0, storage->truncate_prefix(10001));
    ASSERT_EQ(storage->first_log_index(), 10001);
    ASSERT_EQ(storage->last_log_index(), 500000);

    // boundary truncate prefix
    {
        SegmentLogStorage::SegmentMap& segments1 = storage->segments();
        size_t old_segment_num = segments1.size();
        Segment* first_seg = segments1.begin()->second.get();

        ASSERT_EQ(0, storage->truncate_prefix(first_seg->last_index()));
        SegmentLogStorage::SegmentMap& segments2 = storage->segments();
        ASSERT_EQ(old_segment_num, segments2.size());

        ASSERT_EQ(0, storage->truncate_prefix(first_seg->last_index() + 1));
        SegmentLogStorage::SegmentMap& segments3 = storage->segments();
        ASSERT_EQ(old_segment_num - 1, segments3.size());
    }

    ASSERT_EQ(0, storage->truncate_prefix(250001));
    ASSERT_EQ(storage->first_log_index(), 250001);
    ASSERT_EQ(storage->last_log_index(), 500000);
    for (int i = 250001; i <= 500000; i++) {
        int64_t index = i;
        LogEntry* entry = storage->get_entry(index);
        ASSERT_EQ(entry->id.term, 1);
        ASSERT_EQ(entry->type, ENTRY_TYPE_DATA);
        ASSERT_EQ(entry->id.index, index);

        char data_buf[128];
        snprintf(data_buf, sizeof(data_buf), "hello, world: %" PRId64, index);
        ASSERT_EQ(data_buf, entry->data.to_string());
        entry->Release();
    }

    // append
    for (int i = 100000; i < 200000; i++) {
        std::vector<LogEntry*> entries;
        for (int j = 0; j < 5; j++) {
            int64_t index = 5*i + j + 1;
            LogEntry* entry = new LogEntry();
            entry->type = ENTRY_TYPE_DATA;
            entry->id.term = 1;
            entry->id.index = index;

            char data_buf[128];
            snprintf(data_buf, sizeof(data_buf), "hello, world: %" PRId64, index);
            entry->data.append(data_buf);
            entries.push_back(entry);
        }

        ASSERT_EQ(5, storage->append_entries(entries));

        for (size_t j = 0; j < entries.size(); j++) {
            delete entries[j];
        }
    }

    // truncate suffix
    ASSERT_EQ(250001, storage->first_log_index());
    ASSERT_EQ(1000000, storage->last_log_index());
    ASSERT_EQ(0, storage->truncate_suffix(750000));
    ASSERT_EQ(250001, storage->first_log_index());
    ASSERT_EQ(750000, storage->last_log_index());

    // boundary truncate suffix
    {
        SegmentLogStorage::SegmentMap& segments1 = storage->segments();
        Segment* first_seg = segments1.begin()->second.get();
        if (segments1.size() > 1) {
            storage->truncate_suffix(first_seg->last_index() + 1);
        }
        SegmentLogStorage::SegmentMap& segments2 = storage->segments();
        ASSERT_EQ(2ul, segments2.size());
        ASSERT_EQ(storage->last_log_index(), first_seg->last_index() + 1);
        storage->truncate_suffix(first_seg->last_index());
        SegmentLogStorage::SegmentMap& segments3 = storage->segments();
        ASSERT_EQ(1ul, segments3.size());
        ASSERT_EQ(storage->last_log_index(), first_seg->last_index());
    }

    // read
    for (int i = 250001; i <= storage->last_log_index(); i++) {
        int64_t index = i;
        LogEntry* entry = storage->get_entry(index);
        ASSERT_EQ(entry->id.term, 1);
        ASSERT_EQ(entry->type, ENTRY_TYPE_DATA);
        ASSERT_EQ(entry->id.index, index);

        char data_buf[128];
        snprintf(data_buf, sizeof(data_buf), "hello, world: %" PRId64, index);
        ASSERT_EQ(data_buf, entry->data.to_string());
        entry->Release();
    }

    delete storage;

    // re load
    ::system("rm -rf data/log_meta");
    SegmentLogStorage* storage2 = new SegmentLogStorage("./data");
    ASSERT_EQ(0, storage2->init(new ConfigurationManager()));
    ASSERT_EQ(1, storage2->first_log_index());
    ASSERT_EQ(0, storage2->last_log_index());
    delete storage2;
}