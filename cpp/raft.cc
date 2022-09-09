#include <pthread.h>
#include <unistd.h>
#include <butil/string_printf.h>
#include <butil/class_name.h>
#include "raft.h"
#include "node.h"
#include "storage.h"
#include "log.h"
#include "fsm_caller.h"            // IteratorImpl

// ------------- Iterator
void Iterator::next() {
    if (valid()) {
        _impl->next();
    }
}

bool Iterator::valid() const {
    return _impl->is_good() && _impl->entry()->type == ENTRY_TYPE_DATA;
}

int64_t Iterator::index() const { return _impl->index(); }

int64_t Iterator::term() const { return _impl->entry()->id.term; }

const butil::IOBuf& Iterator::data() const {
    return _impl->entry()->data;
}

Closure* Iterator::done() const {
    return _impl->done();
}

void Iterator::set_error_and_rollback(size_t ntail, const butil::Status* st) {
    return _impl->set_error_and_rollback(ntail, st);
}

// ----------------- Default Implementation of StateMachine
StateMachine::~StateMachine() {}
void StateMachine::on_shutdown() {}

void StateMachine::on_leader_start(int64_t) {}
void StateMachine::on_leader_stop(const butil::Status&) {}
void StateMachine::on_error(const Error& e) {
    LOG(ERROR) << "Encountered an error=" << e << " on StateMachine "
               << butil::class_name_str(*this)
               << ", it's highly recommended to implement this interface"
                  " as raft stops working since some error ocurrs,"
                  " you should figure out the cause and repair or remove this node";
}

void StateMachine::on_configuration_committed(const Configuration& conf) {
    (void)conf;
    return;
}

void StateMachine::on_configuration_committed(const Configuration& conf, int64_t index) {
    (void)index;
    return on_configuration_committed(conf);
}

void StateMachine::on_stop_following(const LeaderChangeContext&) {}
void StateMachine::on_start_following(const LeaderChangeContext&) {}
