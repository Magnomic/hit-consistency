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
    node.handle_append_entries_request(cntl, request, response, done_guard.release(), false);
    return; 
}

void RaftSericeImpl::client_request(google::protobuf::RpcController* cntl_base, const ClientRequest* request,
                  ClientResponse* response, google::protobuf::Closure* done){
    brpc::ClosureGuard done_guard(done);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);
    butil::IOBuf log;
    butil::IOBufAsZeroCopyOutputStream wrapper(&log);
    if (!request->SerializeToZeroCopyStream(&wrapper)) {
        LOG(ERROR) << "Fail to serialize request";
        response->set_success(false);
        return;
    }
    // Apply this log as a braft::Task
    Task task;
    task.data = &log;
    // Now the task is applied to the group, waiting for the result.
    NodeImpl& node = NodeImpl::getInstance();
    node.apply_task(task);
    response->set_success(true);
    return LOG(INFO) << "Got request from client";
}