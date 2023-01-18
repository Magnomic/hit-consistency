#include <iostream>
#include <memory>
#include <string>
#include "raft_message.pb.h"
#include "cpp/node.h"
#include "cpp/raft.h"
#include "cpp/configuration.h"
#include "state_machine.pb.h"

#include <sys/types.h>                  // O_CREAT
#include <fcntl.h>                      // open
#include <butil/sys_byteorder.h>        // butil::NetToHost32
#include <butil/logging.h>
#include <brpc/server.h>
#include <gflags/gflags.h>


DEFINE_string(conf, "0.0.0.0:8000:0,0.0.0.0:8001:0,0.0.0.0:8002:0", "Initial configuration of the replication group");
DEFINE_int32(election_timeout_ms, 5000, 
            "Start election in such milliseconds if disconnect with the leader");
DEFINE_bool(echo_attachment, true, "Echo attachment as well");
DEFINE_string(data_path, "data", "Path of data stored on");
DEFINE_int32(port, 8000, "TCP Port of this server");
DEFINE_string(listen_addr, "0.0.0.0", "Server listen address, may be IPV4/IPV6/UDS."
            " If this is set, the flag port will be ignored");
DEFINE_int32(idle_timeout_s, -1, "Connection will be closed if there is no "
             "read/write operations during the last `idle_timeout_s'");
DEFINE_int32(logoff_ms, 2000, "Maximum duration of server's LOGOFF state "
             "(waiting for client to close connection before server stops)");
DEFINE_int32(snapshot_interval, 30, "Interval between each snapshot");
DEFINE_bool(disable_cli, false, "Don't allow raft_cli access this node");
DEFINE_string(group, "Atomic", "Id of the replication group");

using hit_consistency::StateMachineService;
using hit_consistency::StateMachineRequest;
using hit_consistency::StateMachineResponse;

class SampleStateMachine;

// Implements Closure which encloses RPC stuff
class StateMachineClosure : public Closure {

private:
    SampleStateMachine* _ssm;
    butil::IOBuf* _data;
    const StateMachineRequest* _request;
    StateMachineResponse* _response;
    google::protobuf::Closure* _done;

public:
    StateMachineClosure(SampleStateMachine* ssm, 
                    const StateMachineRequest* request,
                    StateMachineResponse* response,
                    butil::IOBuf* data,
                    google::protobuf::Closure* done)
        : _ssm(ssm)
        , _request(request)
        , _response(response)
        , _data(data)
        , _done(done) {}
    ~StateMachineClosure() {}

    const StateMachineRequest* request() const { return _request; }
    StateMachineResponse* response() const { return _response; }
    void Run();
    butil::IOBuf* data() const { return _data; }

};

void StateMachineClosure::Run() {
    // Auto delete this after Run()
    std::unique_ptr<StateMachineClosure> self_guard(this);
    // Repsond this RPC.
    brpc::ClosureGuard done_guard(_done);
    if (status().ok()) {
        return;
    }
}

class SampleStateMachine : public StateMachine {
    

    class SharedFD : public butil::RefCountedThreadSafe<SharedFD> {
    public:
        explicit SharedFD(int fd) : _fd(fd) {}
        int fd() const { return _fd; }
    private:
    friend class butil::RefCountedThreadSafe<SharedFD>;
        ~SharedFD() {
            if (_fd >= 0) {
                while (true) {
                    const int rc = ::close(_fd);
                    if (rc == 0 || errno != EINTR) {
                        break;
                    }
                }
                _fd = -1;
            }
        }
        
        int _fd;
    };

    typedef scoped_refptr<SharedFD> scoped_fd;

    scoped_fd get_fd() const {
        BAIDU_SCOPED_LOCK(_fd_mutex);
        return _fd;
    }

private:
    scoped_fd _fd;
    mutable butil::Mutex _fd_mutex;
    butil::atomic<int64_t> _leader_term;
    NodeImpl* volatile _node;
    std::deque<int64_t> _confliction_offsets;

private:
friend class StateMachineClosure;

public:
    SampleStateMachine() {}
    ~SampleStateMachine() {}

    int start(){
        if (!butil::CreateDirectory(butil::FilePath(FLAGS_data_path))) {
            LOG(ERROR) << "Fail to create directory " << FLAGS_data_path;
            return -1;
        }
        std::string data_path = FLAGS_data_path + "/data";
        int fd = ::open(data_path.c_str(), O_CREAT | O_RDWR, 0644);
        if (fd < 0) {
            PLOG(ERROR) << "Fail to open " << data_path;
            return -1;
        }
        _fd = new SharedFD(fd);

        NodeImpl& node = NodeImpl::getInstance();

        std::string server_addr(FLAGS_listen_addr.c_str());
        
        butil::EndPoint addr(butil::IP_ANY, FLAGS_port);

        NodeOptions node_options;
        if (node_options.initial_conf.parse_from(FLAGS_conf) != 0) {
            LOG(ERROR) << "Fail to parse configuration `" << FLAGS_conf << '\'';
            return -1;
        }
        node_options.election_timeout_ms = FLAGS_election_timeout_ms;
        node_options.node_owns_fsm = false;
        node_options.fsm = this;
        node_options.snapshot_interval_s = FLAGS_snapshot_interval;
        std::string prefix = "local://" + FLAGS_data_path;
        node_options.log_uri = prefix + "/log";
        node_options.raft_meta_uri = prefix + "/raft_meta";
        node_options.snapshot_uri = prefix + "/snapshot";
        node_options.disable_cli = FLAGS_disable_cli;

        node.init(node_options, FLAGS_group, PeerId(addr));

        node.start();

        _node = &node;

        return 0;
    }

    // Shut this node down.
    void shutdown() {
        if (_node) {
            _node->shutdown(NULL);
        }
    }

    // Blocking this thread until the node is eventually down.
    void join() {
        if (_node) {
            _node->join();
        }
    }

    void on_apply(Iterator& iter) {
        // A batch of tasks are committed, which must be processed through 
        // |iter|
        for (; iter.valid(); iter.next()) {
            // LOG(INFO) << "Applying for " << iter.index();
            StateMachineResponse* response = NULL;
            // This guard helps invoke iter.done()->Run() asynchronously to
            // avoid that callback blocks the StateMachine
            AsyncClosureGuard closure_guard(iter.done());
            butil::IOBuf data;
            off_t offset = 0;
            if (iter.done()) {
                // This task is applied by this node, get value from this
                // closure to avoid additional parsing.
                StateMachineClosure* c = dynamic_cast<StateMachineClosure*>(iter.done());
                offset = c->request()->offset();
                data.swap(*(c->data()));
                // LOG(INFO)<<*(c->data());
                response = c->response();
            } else {
                // Have to parse BlockRequest from this log.
                uint32_t meta_size = 0;
                butil::IOBuf saved_log = iter.data();
                saved_log.cutn(&meta_size, sizeof(uint32_t));
                // Remember that meta_size is in network order which hould be
                // covert to host order
                meta_size = butil::NetToHost32(meta_size);
                butil::IOBuf meta;
                saved_log.cutn(&meta, meta_size);
                butil::IOBufAsZeroCopyInputStream wrapper(meta);
                StateMachineRequest request;
                CHECK(request.ParseFromZeroCopyStream(&wrapper));
                data.swap(saved_log);
                offset = request.offset();
            }

            const ssize_t nw = file_pwrite(data, _fd->fd(), offset);
            // const ssize_t nw =0;
            
            if (nw < 0) {
                PLOG(ERROR) << "Fail to write to fd=" << _fd->fd();
                if (response) {
                    response->set_success(false);
                }
                // Let raft run this closure.
                closure_guard.release();
                // Some disk error occurred, notify raft and never apply any data
                // ever after
                iter.set_error_and_rollback();
                return;
            }

            if (response) {
                // LOG(INFO) << "response success";
                response->set_success(true);
            }
        }
    }

    // Impelements Service methods
    void write(const StateMachineRequest* request,
               StateMachineResponse* response,
               butil::IOBuf* data,
               google::protobuf::Closure* done) {
        brpc::ClosureGuard done_guard(done);
        // Serialize request to the replicated write-ahead-log so that all the
        // peers in the group receive this request as well.
        // Notice that _value can't be modified in this routine otherwise it
        // will be inconsistent with others in this group.
        
        // Serialize request to IOBuf
        butil::IOBuf log;
        const uint32_t meta_size_raw = butil::HostToNet32(request->ByteSize());
        log.append(&meta_size_raw, sizeof(uint32_t));
        butil::IOBufAsZeroCopyOutputStream wrapper(&log);
        if (!request->SerializeToZeroCopyStream(&wrapper)) {
            LOG(ERROR) << "Fail to serialize request";
            response->set_success(false);
            return;
        }
        log.append(*data);

        // Apply this log as a braft::Task
        Task task;
        task.data = &log;
        task.dependency_id = request->offset();
        task.expected_term = -1;
        task.done = new StateMachineClosure(this, request, response,
                                     data, done_guard.release());
                                            
        // Now the task is applied to the group, waiting for the result.
        return _node->apply_task(task);
    }

    void read(const StateMachineRequest *request, StateMachineResponse* response,
              butil::IOBuf* buf) {

        if (request->offset() < 0) {
            response->set_success(false);
            return;
        }

        // This is the leader and is up-to-date. It's safe to respond client
        scoped_fd fd = get_fd();
        butil::IOPortal portal;
        const ssize_t nr = file_pread(
                &portal, fd->fd(), request->offset(), request->size());
        if (nr < 0) {
            // Some disk error occurred, shutdown this node and another leader
            // will be elected
            PLOG(ERROR) << "Fail to read from fd=" << fd->fd();
            response->set_success(false);
            return;
        }
        buf->swap(portal);
        if (buf->length() < (size_t)request->size()) {
            buf->resize(request->size());
        }
        response->set_success(true);
    }

};

// Implements example::BlockService if you are using brpc.
class StateMachineServiceImpl : public StateMachineService {
public:
    explicit StateMachineServiceImpl(SampleStateMachine* ssm) : _ssm(ssm) {}
    void write(::google::protobuf::RpcController* controller,
               const ::hit_consistency::StateMachineRequest* request,
               ::hit_consistency::StateMachineResponse* response,
               ::google::protobuf::Closure* done) {
        brpc::Controller* cntl = (brpc::Controller*)controller;
        return _ssm->write(request, response,
                             &cntl->request_attachment(), done);
    }
    void read(::google::protobuf::RpcController* controller,
              const ::hit_consistency::StateMachineRequest* request,
              ::hit_consistency::StateMachineResponse* response,
              ::google::protobuf::Closure* done) {
        brpc::Controller* cntl = (brpc::Controller*)controller;
        brpc::ClosureGuard done_guard(done);
        return _ssm->read(request, response, &cntl->response_attachment());
    }
private:
    SampleStateMachine* _ssm;
};

int main(int argc, char** argv) {


    gflags::ParseCommandLineFlags(&argc, &argv, true);
    
    std::string log_path = std ::to_string(FLAGS_port) + ".log";

    ::logging::LoggingSettings log_setting; 
    log_setting.log_file = log_path.c_str(); 
    log_setting.logging_dest = logging::LOG_TO_FILE; 
    ::logging::InitLogging(log_setting);  

    butil::EndPoint addr(butil::IP_ANY, FLAGS_port);

    brpc::Server server;

    RaftSericeImpl raft_service_impl;
    
    if (server.AddService(&raft_service_impl, 
                            brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
            LOG(ERROR) << "Fail to add service";
            return -1;
        }

    SampleStateMachine ssm;
    StateMachineServiceImpl service(&ssm);

    if (server.AddService(&service, 
                            brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
            LOG(ERROR) << "Fail to add service";
            return -1;
        }

    // Start the server.
    brpc::ServerOptions options;


    options.idle_timeout_sec = 0;

    if (server.Start(addr, &options) != 0) {
        LOG(ERROR) << "Fail to start EchoServer";
        return -1;
    }

    // It's ok to start Block
    if (ssm.start() != 0) {
        LOG(ERROR) << "Fail to start Block";
        return -1;
    }

    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    while (!brpc::IsAskedToQuit()) {
        sleep(1);
    }
    LOG(INFO) << "Block service is going to quit";

    ssm.shutdown();
     
    server.Stop(0);

    ssm.join();

    server.Join();

    return 0;

}