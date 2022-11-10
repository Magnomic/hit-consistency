
#ifndef  BRAFT_CLOSURE_QUEUE_H
#define  BRAFT_CLOSURE_QUEUE_H

#include "util.h"

class Closure;

class ClosureQueue {
public:
    explicit ClosureQueue(bool usercode_in_pthread);
    ~ClosureQueue();

    // Clear all the pending closure and run done
    void clear();

    // Called when a candidate becomes the new leader, otherwise the behavior is
    // undefined.
    // Reset the first index of the coming pending closures to |first_index|
    void reset_first_index(int64_t first_index);

    // Called by leader, otherwise the behavior is undefined
    // Append the closure
    void append_pending_closure(Closure* c);

    // Pop all the closure until |index| (included) into out in the same order
    // of thier indexes, |out_first_index| would be assigned the index of out[0] if
    // out is not empyt, index + 1 otherwise.
    /* Commmit from from_index to to_index. Pop entries from front if they are sequential |true| in _has_committed_queue. */
    int pop_closure_until(int64_t from_index, int64_t to_index, 
                          std::vector<Closure*> *out, int64_t *out_first_index, std::deque<int64_t> *index_list);
private:
    // TODO: a spsc lock-free queue would help
    raft::raft_mutex_t                                    _mutex;
    int64_t                                         _first_index;
    std::deque<Closure*>                            _queue;
    std::deque<bool>                                _has_committed_queue;
    bool                                            _usercode_in_pthread;

};

#endif  //BRAFT_CLOSURE_QUEUE_H
