#include"fsync.h"
#include <brpc/reloadable_flags.h>  //BRPC_VALIDATE_GFLAG


DEFINE_bool(raft_use_fsync_rather_than_fdatasync,
            true,
            "Use fsync rather than fdatasync to flush page cache");

BRPC_VALIDATE_GFLAG(raft_use_fsync_rather_than_fdatasync,
                         brpc::PassValidate);
