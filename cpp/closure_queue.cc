#include <bthread/unstable.h>
#include "closure_queue.h"
#include "raft.h"

ClosureQueue::ClosureQueue(bool usercode_in_pthread) 
    : _first_index(0)
    , _usercode_in_pthread(usercode_in_pthread)
{}

ClosureQueue::~ClosureQueue() {
    clear();
}

void ClosureQueue::clear() {
    std::deque<Closure*> saved_queue;
    {
        BAIDU_SCOPED_LOCK(_mutex);
        saved_queue.swap(_queue);
        _first_index = 0;
    }
    bool run_bthread = false;
    for (std::deque<Closure*>::iterator 
            it = saved_queue.begin(); it != saved_queue.end(); ++it) {
        if (*it) {
            (*it)->status().set_error(EPERM, "leader stepped down");
            run_closure_in_bthread_nosig(*it, _usercode_in_pthread);
            run_bthread = true;
        }
    }
    if (run_bthread) {
        bthread_flush();
    }
}

void ClosureQueue::reset_first_index(int64_t first_index) {
    BAIDU_SCOPED_LOCK(_mutex);
    CHECK(_queue.empty());
    _first_index = first_index;
}

void ClosureQueue::append_pending_closure(Closure* c) {
    BAIDU_SCOPED_LOCK(_mutex);
    _queue.push_back(c);
    _has_committed_queue.push_back(false);
}

int ClosureQueue::pop_closure_until(int64_t to_index, std::deque<int64_t> _oo_committed_entries, 
                                    std::vector<Closure*> *out, std::deque<int64_t> *out_indexes) {
    out->clear();
    BAIDU_SCOPED_LOCK(_mutex);
    if (_queue.empty() || !_oo_committed_entries.empty() && _oo_committed_entries.back() < _first_index) {
        *out_indexes = _oo_committed_entries;
        return 0;
    }
    // LOG(INFO) << " _first_index=" << _first_index
    //             << " to_index=" << to_index
    //             << " _closure_queue_size=" << _queue.size();
    // if (!_oo_committed_entries.empty()){
    //     LOG(INFO)   << " _oo_committed_entries.size=" << _oo_committed_entries.size();
    //     LOG(INFO)   << " _oo_committed_entries first index=" << _oo_committed_entries.front();
    //  }
    if (!_oo_committed_entries.empty() && _oo_committed_entries.back() > _first_index + (int64_t)_queue.size() - 1) {
        CHECK(false) << "Invalid index=" << to_index
                     << " _first_index=" << _first_index
                     << " _closure_queue_size=" << _queue.size();
        return -1;
    }
    int64_t ite = _first_index;
    int64_t queue_index;
    /* If commit ooEntries*/
    while (!_oo_committed_entries.empty() || ite <= to_index){
        if (ite <= to_index){
            // if (!_has_committed_queue[ite - _first_index]){
            //     out->push_back(_queue[ite - _first_index]);
            //     out_indexes->push_back(ite);
            //     _has_committed_queue[ite - _first_index] = true;
            // }
            queue_index = ite - _first_index;
            ite++;
        } else {
            queue_index = _oo_committed_entries.front() - _first_index;
            // out->push_back(_queue[_oo_committed_entries.front() - _first_index]);
            // out_indexes->push_back(_oo_committed_entries.front());
            // _has_committed_queue[_oo_committed_entries.front() - _first_index] = true;
            _oo_committed_entries.pop_front();
        }
        if (!_has_committed_queue[queue_index]){
                out->push_back(_queue[queue_index]);
                out_indexes->push_back(queue_index + _first_index);
                _has_committed_queue[queue_index] = true;
        }
    }
    while (!_has_committed_queue.empty() && _has_committed_queue.front()){
        _queue.pop_front();
        _has_committed_queue.pop_front();
        _first_index++;
    }
    // LOG(INFO) << " _first_index=" << _first_index
    //             << " _closure_queue_size=" << _queue.size();
    // for (int64_t i = *out_first_index; i <= to_index; ++i) {
    //     out->push_back(_queue.front());
    //     _queue.pop_front();
    // }
    // _first_index = index + 1;
    return 0;
}