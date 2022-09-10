#include "raft_service.h"

#include "node.h"

RaftSericeImpl::RaftSericeImpl(){

}

RaftSericeImpl::~RaftSericeImpl(){
    
}

void RaftSericeImpl::prevote(google::protobuf::RpcController* cntl_base, const RequestVoteRequest* request,
                  RequestVoteResponse* response, google::protobuf::Closure* done) {
    brpc::ClosureGuard done_guard(done);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);
    NodeImpl& node = NodeImpl::getInstance();
    node.handle_prevote(request, response);
    LOG(INFO) << "Get prevote from " << request->server_id();
    return;
}

void RaftSericeImpl::request_vote(google::protobuf::RpcController* cntl_base, const RequestVoteRequest* request,
                  RequestVoteResponse* response, google::protobuf::Closure* done){
    brpc::ClosureGuard done_guard(done);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);
    NodeImpl& node = NodeImpl::getInstance();
    node.handle_request_vote(request, response);
    LOG(INFO) << "Get vote from " << request->server_id();
    return; 
}

void RaftSericeImpl::append_entries(google::protobuf::RpcController* cntl_base, const AppendEntriesRequest* request,
                  AppendEntriesResponse* response, google::protobuf::Closure* done){
    brpc::ClosureGuard done_guard(done);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);
    NodeImpl& node = NodeImpl::getInstance();
    node.handle_append_entries_request(cntl, request, response, done, false);
    LOG(INFO) << "Get AppendEntries from " << request->server_id();
    return; 
}