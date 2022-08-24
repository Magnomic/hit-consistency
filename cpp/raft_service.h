#ifndef _raft_serivce_H_
#define _raft_serivce_H_
#include <iostream>
#include <memory>
#include <string>
#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include "raft_message.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using hit_consistency::RaftService;
using hit_consistency::RequestVoteRequest;
using hit_consistency::RequestVoteResponse;


class RaftSericeImpl final : public RaftService::Service {
    public:
    RaftSericeImpl();
    ~RaftSericeImpl();
    Status prevote(ServerContext* context, const RequestVoteRequest* request,
                  RequestVoteResponse* reply);
};

#endif 