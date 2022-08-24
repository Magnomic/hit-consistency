#!/bin/bash

protoc -I . --cpp_out=. raft_message.proto
protoc -I . --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` raft_message.proto
