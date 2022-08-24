#!/bin/bash

protoc -I ../../protos --cpp_out=. ../../protos/raft_message.proto
protoc -I ../../protos --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ../../protos/raft_message.proto
