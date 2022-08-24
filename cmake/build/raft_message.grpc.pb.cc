// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: raft_message.proto

#include "raft_message.pb.h"
#include "raft_message.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace hit_consistency {

static const char* RaftService_method_names[] = {
  "/hit_consistency.RaftService/prevote",
};

std::unique_ptr< RaftService::Stub> RaftService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< RaftService::Stub> stub(new RaftService::Stub(channel, options));
  return stub;
}

RaftService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_prevote_(RaftService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status RaftService::Stub::prevote(::grpc::ClientContext* context, const ::hit_consistency::RequestVoteRequest& request, ::hit_consistency::RequestVoteResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::hit_consistency::RequestVoteRequest, ::hit_consistency::RequestVoteResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_prevote_, context, request, response);
}

void RaftService::Stub::async::prevote(::grpc::ClientContext* context, const ::hit_consistency::RequestVoteRequest* request, ::hit_consistency::RequestVoteResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::hit_consistency::RequestVoteRequest, ::hit_consistency::RequestVoteResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_prevote_, context, request, response, std::move(f));
}

void RaftService::Stub::async::prevote(::grpc::ClientContext* context, const ::hit_consistency::RequestVoteRequest* request, ::hit_consistency::RequestVoteResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_prevote_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::hit_consistency::RequestVoteResponse>* RaftService::Stub::PrepareAsyncprevoteRaw(::grpc::ClientContext* context, const ::hit_consistency::RequestVoteRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::hit_consistency::RequestVoteResponse, ::hit_consistency::RequestVoteRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_prevote_, context, request);
}

::grpc::ClientAsyncResponseReader< ::hit_consistency::RequestVoteResponse>* RaftService::Stub::AsyncprevoteRaw(::grpc::ClientContext* context, const ::hit_consistency::RequestVoteRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncprevoteRaw(context, request, cq);
  result->StartCall();
  return result;
}

RaftService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      RaftService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< RaftService::Service, ::hit_consistency::RequestVoteRequest, ::hit_consistency::RequestVoteResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](RaftService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::hit_consistency::RequestVoteRequest* req,
             ::hit_consistency::RequestVoteResponse* resp) {
               return service->prevote(ctx, req, resp);
             }, this)));
}

RaftService::Service::~Service() {
}

::grpc::Status RaftService::Service::prevote(::grpc::ServerContext* context, const ::hit_consistency::RequestVoteRequest* request, ::hit_consistency::RequestVoteResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace hit_consistency

