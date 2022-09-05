#ifndef  BRAFT_FSYNC_H
#define  BRAFT_FSYNC_H

#include <unistd.h>
#include <fcntl.h>
#include <gflags/gflags.h>
#include "storage.h"


DECLARE_bool(raft_use_fsync_rather_than_fdatasync);

inline int raft_fsync(int fd) {
    if (FLAGS_raft_use_fsync_rather_than_fdatasync) {
        return fsync(fd);
    } else {
#ifdef __APPLE__
        return fcntl(fd, F_FULLFSYNC);
#else
        return fdatasync(fd);
#endif
    }
}

inline bool raft_sync_meta() {
    return FLAGS_raft_sync || FLAGS_raft_sync_meta;
}


#endif  //BRAFT_FSYNC_H