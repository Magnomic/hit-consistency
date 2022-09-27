# hit-consistency
Opensource implementions of consistency protocols using [bRPC](https://github.com/apache/incubator-brpc).

## Problem statement
When we use a multi-thread client to run the Raft protocol, e.g., 16 threads, the out-of-ordered replication requests leads to more than 30% qps decreases than the cache-enabled replication strategy.