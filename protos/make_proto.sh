#!/bin/bash

protoc -I . --cpp_out=. raft_message.proto
