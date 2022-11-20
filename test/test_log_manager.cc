#define private public
#include <gtest/gtest.h>

#include <butil/memory/scoped_ptr.h>
#include <butil/string_printf.h>
#include <butil/macros.h>

#include <bthread/countdown_event.h>
#include "cpp/log_manager.h"
#include "cpp/configuration.h"
#include "cpp/log.h"

class LogManagerTest : public testing::Test {
protected:
    LogManagerTest() {}
    void SetUp() { }
    void TearDown() { }
};


class SyncClosure : public LogManager::StableClosure {
public:
    SyncClosure() : _event(1) {}
    ~SyncClosure() {
    }
    void Run() {
        _event.signal();
    }
    void reset() {
        status().reset();
        _event.reset();
    }
    void join() {
        _event.wait();
    }
private:
    bthread::CountdownEvent _event;
};


TEST_F(LogManagerTest, append_with_the_same_index) {
    system("rm -rf ./data");
    scoped_ptr<ConfigurationManager> cm(
                                new ConfigurationManager);
    scoped_ptr<SegmentLogStorage> storage(
                                new SegmentLogStorage("./data"));
    scoped_ptr<LogManager> lm(new LogManager());
    LogManagerOptions opt;
    opt.log_storage = storage.get();
    opt.configuration_manager = cm.get();
    ASSERT_EQ(0, lm->init(opt));
    const size_t N = 1000;
    std::vector<LogEntry*> entries0;
    for (size_t i = 50; i < N; ++i) {
        LogEntry* entry = new LogEntry;
        entry->AddRef();
        entry->type = ENTRY_TYPE_DATA;
        std::string buf;
        butil::string_printf(&buf, "hello_%lu", i);
        entry->data.append(buf);
        entry->id = LogId(i + 1, 1);
        entries0.push_back(entry);
        entry->AddRef();
    }
    std::vector<LogEntry*> entries_oo;
    for (size_t i = 0; i < 50; ++i) {
        LogEntry* entry = new LogEntry;
        entry->AddRef();
        entry->type = ENTRY_TYPE_DATA;
        std::string buf;
        butil::string_printf(&buf, "hello_%lu", i);
        entry->data.append(buf);
        entry->id = LogId(i + 1, 1);
        entries_oo.push_back(entry);
        entry->AddRef();
    }
    std::vector<LogEntry*> saved_entries0(entries0);
    SyncClosure sc;
    lm->append_entries(&entries0, &sc, true);
    sc.join();
    SyncClosure sc_oo;
    lm->append_entries(&entries_oo, &sc_oo, true);
    sc_oo.join();
    ASSERT_TRUE(sc.status().ok()) << sc.status();
    ASSERT_EQ(N, lm->last_log_index());

    // Append the same logs, should be ok
    std::vector<LogEntry*> entries1;
    for (size_t i = 50; i < N; ++i) {
        LogEntry* entry = new LogEntry;
        entry->AddRef();
        entry->type = ENTRY_TYPE_DATA;
        std::string buf;
        butil::string_printf(&buf, "hello_%lu", i);
        entry->data.append(buf);
        entry->id = LogId(i + 1, 1);
        entries1.push_back(entry);
        entry->AddRef();
    }

    std::vector<LogEntry*> saved_entries1(entries1);
    sc.reset();
    lm->append_entries(&entries1, &sc);
    sc.join();
    ASSERT_TRUE(sc.status().ok()) << sc.status();
    ASSERT_EQ(N, lm->last_log_index());
    for (size_t i = 0; i < N - 50; ++i) {
        ASSERT_EQ(3u, saved_entries0[i]->ref_count_ + saved_entries1[i]->ref_count_);
    }

    // new term should overwrite the old ones
    std::vector<LogEntry*> entries2;
    for (size_t i = 50; i < N; ++i) {
        LogEntry* entry = new LogEntry;
        entry->AddRef();
        entry->type = ENTRY_TYPE_DATA;
        std::string buf;
        butil::string_printf(&buf, "hello_%lu", (i + 1) * 10);
        entry->data.append(buf);
        entry->id = LogId(i + 1, 2);
        entries2.push_back(entry);
        entry->AddRef();
    }
    std::vector<LogEntry*> saved_entries2(entries2);
    sc.reset();
    lm->append_entries(&entries2, &sc);
    sc.join();
    ASSERT_TRUE(sc.status().ok()) << sc.status();
    ASSERT_EQ(N, lm->last_log_index());

    for (size_t i = 0; i < N - 50; ++i) {
        ASSERT_EQ(1u, saved_entries0[i]->ref_count_);
        ASSERT_EQ(1u, saved_entries1[i]->ref_count_);
        ASSERT_EQ(2u, saved_entries2[i]->ref_count_);
    }

    for (size_t i = 50; i < N; ++i) {
        LogEntry* entry = lm->get_entry(i + 1);
        ASSERT_TRUE(entry != NULL);
        std::string buf;
        butil::string_printf(&buf, "hello_%lu", (i + 1) * 10);
        ASSERT_EQ(buf, entry->data.to_string());
        ASSERT_EQ(LogId(i + 1, 2), entry->id);
        entry->Release();
    }
    lm->set_applied_id(LogId(N, 2));
    usleep(100 * 1000l);

    for (size_t i = 0; i < N - 50; ++i) {
        ASSERT_EQ(1u, saved_entries0[i]->ref_count_);
        ASSERT_EQ(1u, saved_entries1[i]->ref_count_);
        ASSERT_EQ(1u, saved_entries2[i]->ref_count_);
    }

    for (size_t i = 50; i < N; ++i) {
        LogEntry* entry = lm->get_entry(i + 1);
        ASSERT_TRUE(entry != NULL);
        std::string buf;
        butil::string_printf(&buf, "hello_%lu", (i + 1) * 10);
        ASSERT_EQ(buf, entry->data.to_string());
        ASSERT_EQ(LogId(i + 1, 2), entry->id);
        entry->Release();
    }

    for (size_t i = 0; i < N - 50; ++i) {
        saved_entries0[i]->Release();
        saved_entries1[i]->Release();
        saved_entries2[i]->Release();
    }
}
