#include "raft_service.h"

RaftSericeImpl::RaftSericeImpl(){

}

RaftSericeImpl::~RaftSericeImpl(){
    
}

Status RaftSericeImpl::prevote(ServerContext* context, const RequestVoteRequest* request,
        RequestVoteResponse* reply) {
            std::string prifix("Hello");
            reply->set_granted(true);
            reply->set_term(1);
            return Status::OK;
}