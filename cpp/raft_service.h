#ifndef _raft_serivce_H_
#define _raft_serivce_H_
#include <iostream>
#include <memory>
#include <string>
#include <butil/logging.h>
#include <brpc/server.h>
#include <brpc/channel.h>
#include "raft_message.pb.h"

using hit_consistency::RaftService;
using hit_consistency::RequestVoteRequest;
using hit_consistency::RequestVoteResponse;
using hit_consistency::AppendEntriesRequest;
using hit_consistency::AppendEntriesResponse;


class RaftSericeImpl final : public RaftService {
    public:
    RaftSericeImpl();
    ~RaftSericeImpl();
    void prevote(google::protobuf::RpcController* cntl_base, const RequestVoteRequest* request,
                  RequestVoteResponse* response, google::protobuf::Closure* done);
    void request_vote(google::protobuf::RpcController* cntl_base, const RequestVoteRequest* request,
                  RequestVoteResponse* response, google::protobuf::Closure* done);
    void append_entries(google::protobuf::RpcController* cntl_base, const AppendEntriesRequest* request,
                  AppendEntriesResponse* response, google::protobuf::Closure* done);
};

#endif 