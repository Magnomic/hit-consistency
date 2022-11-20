#ifndef BRAFT_LOG_H
#define BRAFT_LOG_H

#include <vector>
#include <map>
#include <butil/memory/ref_counted.h>
#include <butil/atomicops.h>
#include <butil/iobuf.h>
#include <butil/logging.h>

#include "macros.h"
#include "log_entry.h"
#include "storage.h"

using hit_consistency::LogPBMeta;

class BAIDU_CACHELINE_ALIGNMENT Segment 
        : public butil::RefCountedThreadSafe<Segment> {
public:
    Segment(const std::string& path, const int64_t first_index, int checksum_type)
        : _path(path), _bytes(0),
        _fd(-1), _is_open(true), _pending_for_closing(false),
        _first_index(first_index), _last_index(first_index - 1), _vaild_entries_counter(0),
        _checksum_type(checksum_type), _offset_and_term_array(1000000, std::make_pair(-1, -1)), _end_offset_and_term_array(1000000, std::make_pair(-1, -1))
    {}
    Segment(const std::string& path, const int64_t first_index, const int64_t last_index,
            int checksum_type)
        : _path(path), _bytes(0),
        _fd(-1), _is_open(false), _pending_for_closing(false),
        _first_index(first_index), _last_index(last_index), _vaild_entries_counter(0),
        _checksum_type(checksum_type), _offset_and_term_array(1000000, std::make_pair(-1, -1)), _end_offset_and_term_array(1000000, std::make_pair(-1, -1))
    {}

    struct EntryHeader;

    // create open segment
    int create();

    // load open or closed segment
    // open fd, load index, truncate uncompleted entry
    int load(ConfigurationManager* configuration_manager);

    // serialize entry, and append to open segment
    int append(const LogEntry* entry);

    // get entry by index
    LogEntry* get(const int64_t index) const;

    // get entry's term by index
    int64_t get_term(const int64_t index) const;

    // close open segment
    int close(bool will_sync = true);

    // sync open segment
    int sync(bool will_sync);

    // unlink segment
    int unlink();

    // truncate segment to last_index_kept
    int truncate(const int64_t last_index_kept);

    bool is_open() const {
        return _is_open;
    }

    int prepare_to_close() {
        _pending_for_closing = true;
        return 0;
    }

    bool is_fullfilled() const {
        return _last_index - _first_index + 1 - _vaild_entries_counter == 0;
    }

    int64_t bytes() const {
        return _bytes;
    }

    int64_t first_index() const {
        return _first_index;
    }

    int64_t last_index() const {
        return _last_index.load(butil::memory_order_consume);
    }

    std::string file_name();
private:
friend class butil::RefCountedThreadSafe<Segment>;
    ~Segment() {
        if (_fd >= 0) {
            ::close(_fd);
            _fd = -1;
        }
    }

    struct LogMeta {
        off_t offset;
        size_t length;
        int64_t term;
    };

    int _load_entry(off_t offset, EntryHeader *head, butil::IOBuf *body, 
                    size_t size_hint) const;

    int _get_meta(int64_t index, LogMeta* meta) const;

    int _truncate_meta_and_get_last(int64_t last);

    std::string _path;
    int64_t _bytes;
    mutable raft::raft_mutex_t _mutex;
    int _fd;
    bool _is_open;
    bool _pending_for_closing;
    bool _sequential;
    const int64_t _first_index;
    butil::atomic<int64_t> _last_index;
    butil::atomic<int64_t> _vaild_entries_counter;
    int _checksum_type;
    // 这个vector必须是有序的
    std::vector<std::pair<int64_t/*offset*/, int64_t/*term*/> > _offset_and_term;
    std::vector<std::pair<int64_t/*offset*/, int64_t/*term*/>> _offset_and_term_array;
    std::vector<std::pair<int64_t/*offset*/, int64_t/*term*/>> _end_offset_and_term_array;
    // 虽然我没感觉这个term有啥用，但是还是加进去吧
    std::vector<std::pair<int64_t/*end_offset*/, int64_t/*index*/> > _end_offset_and_index;
};

class SegmentLogStorage : public LogStorage {
public:
    typedef std::map<int64_t, scoped_refptr<Segment> > SegmentMap;

    explicit SegmentLogStorage(const std::string& path, bool enable_sync = true)
        : _path(path)
        , _first_log_index(1)
        , _last_log_index(0)
        , _max_log_index(0)
        , _checksum_type(0)
        , _enable_sync(enable_sync)
    {} 

    SegmentLogStorage()
        : _first_log_index(1)
        , _last_log_index(0)
        , _max_log_index(0)
        , _checksum_type(0)
        , _enable_sync(true)
    {}

    virtual ~SegmentLogStorage() {}

    // init logstorage, check consistency and integrity
    virtual int init(ConfigurationManager* configuration_manager);

    // first log index in log
    virtual int64_t first_log_index() {
        return _first_log_index.load(butil::memory_order_acquire);
    }

    // last log index in log
    virtual int64_t last_log_index();

    virtual int64_t max_log_index();

    // get logentry by index
    virtual LogEntry* get_entry(const int64_t index);

    // get logentry's term by index
    virtual int64_t get_term(const int64_t index);

    // append entry to log
    int append_entry(const LogEntry* entry);

    // append entries to log, return success append number
    virtual int append_entries(const std::vector<LogEntry*>& entries);

    // delete logs from storage's head, [1, first_index_kept) will be discarded
    virtual int truncate_prefix(const int64_t first_index_kept);

    // delete uncommitted logs from storage's tail, (last_index_kept, infinity) will be discarded
    virtual int truncate_suffix(const int64_t last_index_kept);

    virtual int reset(const int64_t next_log_index);

    LogStorage* new_instance(const std::string& uri) const;

    SegmentMap& segments() {
        return _segments;
    }

    void list_files(std::vector<std::string>* seg_files);

    void sync();
private:
    scoped_refptr<Segment> open_segment();
    int save_meta(const int64_t log_index);
    int load_meta();
    int list_segments(bool is_empty);
    int load_segments(ConfigurationManager* configuration_manager);
    int get_segment(int64_t log_index, scoped_refptr<Segment>* ptr);
    void pop_segments(
            int64_t first_index_kept, 
            std::vector<scoped_refptr<Segment> >* poped);
    void pop_segments_from_back(
            const int64_t first_index_kept,
            std::vector<scoped_refptr<Segment> >* popped,
            scoped_refptr<Segment>* last_segment);


    std::string _path;
    butil::atomic<int64_t> _first_log_index;
    butil::atomic<int64_t> _last_log_index;
    butil::atomic<int64_t> _max_log_index;
    raft::raft_mutex_t _mutex;
    SegmentMap _segments;
    scoped_refptr<Segment> _open_segment;
    int _checksum_type;
    bool _enable_sync;
};
#endif //~BRAFT_LOG_H