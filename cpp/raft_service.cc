#include "raft_service.h"

RaftSericeImpl::RaftSericeImpl(){

}

RaftSericeImpl::~RaftSericeImpl(){
    
}

void RaftSericeImpl::prevote(google::protobuf::RpcController* cntl_base, const RequestVoteRequest* request,
                  RequestVoteResponse* response, google::protobuf::Closure* done) {
    brpc::ClosureGuard done_guard(done);
    brpc::Controller* cntl =
    static_cast<brpc::Controller*>(cntl_base);
    NodeImpl& node = NodeImpl::getInstance();
    node.handle_prevote(request, response);
    LOG(INFO) << "Get prevote from " << request->server_id();
    response->set_granted(true);
    response->set_term(1);
    return;
}