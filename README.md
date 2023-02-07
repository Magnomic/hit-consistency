# CP-Raft implementation

This work is an opensource implementation of [CP-Raft](https://github.com/Magnomic/CP-Raft): Concise and Generalized Design of Parallel Raft Protocol based on [bRPC](https://github.com/apache/incubator-brpc). We submitted our paper to ICDCS-2023 (under review.)

The experimental results show that CP-Raft can improve transaction per second (TPS) performance by 30\%-90\%X and reduce response latency by 30\%-50\% than Raft cache and LCR. It also provides better availability than state-of-the-art OORaft protocols.

For correctness verification, please see [CP-Raft](https://github.com/Magnomic/CP-Raft). This repository only provides the a C++ implementation of the protocol. Considering industrial practice experience, this code does not strictly follow the TLA+ code (e.g., Gap entries, Confirmed gap entries will not actually be saved as log entries in disk or memory because they can be calculated and saved in metadata of leader)
