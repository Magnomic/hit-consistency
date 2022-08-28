#ifndef _node_H_
#define _node_H_

#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <functional>
#include <set>

#include <butil/logging.h>
#include <butil/atomic_ref_count.h>
#include <butil/memory/ref_counted.h>
#include <butil/iobuf.h>
#include <brpc/server.h>
#include <brpc/channel.h>

#include "raft_message.pb.h"

#include "raft_service.h"
#include "replicator.h"
#include "configuration.h"
#include "repeated_timer_task.h"

using hit_consistency::RaftService;
using hit_consistency::RaftService_Stub;
using hit_consistency::RequestVoteRequest;
using hit_consistency::RequestVoteResponse;

class NodeImpl;

class NodeTimer : public RepeatedTimerTask {
    public:
        NodeTimer() : _node(NULL) {}
        virtual ~NodeTimer() {}
        int init(NodeImpl* node, int timeout_ms);
        virtual void run() = 0;
    protected:
        void on_destroy();
        NodeImpl* _node;
};

class ElectionTimer : public NodeTimer {
    protected:
        void run();
        int adjust_timeout_ms(int timeout_ms);
};

class NodeImpl : public butil::RefCountedThreadSafe<NodeImpl>{

    private:

        friend class butil::RefCountedThreadSafe<NodeImpl>;

        std::set<PeerId> _peer_list;

        GroupId _group_id;
        
        PeerId _server_id;

        int _server_timeout;

        ElectionTimer _election_timer;

        raft::raft_mutex_t _mutex;

        NodeImpl();

        ~NodeImpl();

        NodeImpl(const NodeImpl&);

        NodeImpl& operator=(const NodeImpl&);

    public:        
        NodeId node_id() const {
            return NodeId(_group_id, _server_id);
        }
        static NodeImpl& getInstance();

        int init(std::string server_addr, int server_port, int server_timeout);

        int start();

        void prevote(std::unique_lock<raft::raft_mutex_t>* lck);

        int handle_prevote(const RequestVoteRequest* request, RequestVoteResponse* response);

        void handle_pre_vote_response(const PeerId& peer_id_, const int64_t term_, RequestVoteResponse response);

        void handle_election_timeout();

};
#endif 