#ifndef _raft_serivce_H_
#define _raft_serivce_H_
#include <iostream>
#include <memory>
#include <string>
#include <butil/logging.h>
#include <brpc/server.h>
#include <brpc/channel.h>
#include "raft_message.pb.h"
#include "node.h"

using hit_consistency::RaftService;
using hit_consistency::RequestVoteRequest;
using hit_consistency::RequestVoteResponse;


class RaftSericeImpl final : public RaftService {
    public:
    RaftSericeImpl();
    ~RaftSericeImpl();
    void prevote(google::protobuf::RpcController* cntl_base, const RequestVoteRequest* request,
                  RequestVoteResponse* response, google::protobuf::Closure* done);
};

#endif 