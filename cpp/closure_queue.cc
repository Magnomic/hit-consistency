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

int ClosureQueue::pop_closure_until(int64_t from_index, int64_t to_index, 
                                    std::vector<Closure*> *out, int64_t *out_first_index, std::deque<int64_t> *index_list) {
    out->clear();
    BAIDU_SCOPED_LOCK(_mutex);
    if (_queue.empty() || to_index < _first_index) {
        *out_first_index = to_index + 1;
        return 0;
    }
    if (to_index > _first_index + (int64_t)_queue.size() - 1) {
        CHECK(false) << "Invalid index=" << to_index
                     << " _first_index=" << _first_index
                     << " _closure_queue_size=" << _queue.size();
        return -1;
    }
    *out_first_index = std::max(_first_index, from_index);
    std::deque<Closure*>::iterator it_clo(_queue.begin());
    std::deque<bool>::iterator it_com(_has_committed_queue.begin());
    /* If commit ooEntries*/
    if (from_index > _first_index){
        it_clo += from_index - _first_index;
        it_com += from_index - _first_index;
        for (int64_t i = from_index; i < to_index; i++){
            if (!*it_com){
                out->push_back(*it_clo);
                index_list->push_back(i);
                *it_com = true;
            } 
            it_clo++;
            it_com++;
        }
    } else {
        for (int64_t i = *out_first_index; i <= to_index; ++i) {
            out->push_back(_queue.front());
            _queue.pop_front();
            _has_committed_queue.pop_front();
        }
        _first_index = to_index + 1;
    }
    while (_has_committed_queue.front()){
        _queue.pop_front();
        _has_committed_queue.pop_front();
        _first_index++;
    }
    // for (int64_t i = *out_first_index; i <= to_index; ++i) {
    //     out->push_back(_queue.front());
    //     _queue.pop_front();
    // }
    // _first_index = index + 1;
    return 0;
}