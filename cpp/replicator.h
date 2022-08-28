#ifndef _replicator_H_
#define _replicator_H_

#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <functional>

#include "raft_message.pb.h"

#include "raft_service.h"

using hit_consistency::RaftService;
using hit_consistency::RequestVoteRequest;
using hit_consistency::RequestVoteResponse;

class Replicator {

    private:
        Replicator();

        ~Replicator();

        Replicator(const Replicator&);

        Replicator& operator=(const Replicator&);

        Replicator& _timer = Replicator::getInstance();

        int _send_empty_entries();

    public:
    
        std::unique_ptr<RaftService::Stub> _stub;

        static Replicator& getInstance();

        void run(std::function<int(void)> f, int duration);

};

#endif 