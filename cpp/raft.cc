#include <pthread.h>
#include <unistd.h>
#include <butil/string_printf.h>
#include <butil/class_name.h>
#include "raft.h"
#include "node.h"
#include "storage.h"
#include "log.h"
#include "fsm_caller.h"            // IteratorImpl

static void print_revision(std::ostream& os, void*) {
#if defined(BRAFT_REVISION)
        os << BRAFT_REVISION;
#else
        os << "undefined";
#endif
}

static bvar::PassiveStatus<std::string> s_raft_revision(
        "raft_revision", print_revision, NULL);


static pthread_once_t global_init_once = PTHREAD_ONCE_INIT;

struct GlobalExtension {
    SegmentLogStorage local_log;
    LocalRaftMetaStorage local_meta;
};

static void global_init_or_die_impl() {
    static GlobalExtension s_ext;

    log_storage_extension()->RegisterOrDie("local", &s_ext.local_log);
    meta_storage_extension()->RegisterOrDie("local", &s_ext.local_meta);
}

// Non-static for unit test
void global_init_once_or_die() {
    if (pthread_once(&global_init_once, global_init_or_die_impl) != 0) {
        PLOG(FATAL) << "Fail to pthread_once";
        exit(-1);
    }
}

Node::Node(const GroupId& group_id, const PeerId& peer_id) {
    _impl = new NodeImpl(group_id, peer_id);
}

Node::~Node() {

}

NodeId Node::node_id() {
}

PeerId Node::leader_id() {
}

bool Node::is_leader() {
}

int Node::init(const NodeOptions& options) {
}

void Node::shutdown(Closure* done) {
}

void Node::join() {
}

void Node::apply(const Task& task) {
}

butil::Status Node::list_peers(std::vector<PeerId>* peers) {
}

void Node::add_peer(const PeerId& peer, Closure* done) {
}

void Node::remove_peer(const PeerId& peer, Closure* done) {
}

void Node::change_peers(const Configuration& new_peers, Closure* done) {
}

butil::Status Node::reset_peers(const Configuration& new_peers) {
}

void Node::snapshot(Closure* done) {
}

void Node::vote(int election_timeout) {
}

void Node::reset_election_timeout_ms(int election_timeout_ms) {
}

int Node::transfer_leadership_to(const PeerId& peer) {
}

void Node::enter_readonly_mode() {
}

void Node::leave_readonly_mode() {
}

bool Node::readonly() {
}

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
