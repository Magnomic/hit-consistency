cmake_minimum_required(VERSION 2.8.10)
project(echo_c++ C CXX)

option(LINK_SO "Whether examples are linked dynamically" OFF)

execute_process(
    COMMAND bash -c "find ../incubator-brpc -type d -regex \".*output/include$\" | head -n1 | xargs dirname | tr -d '\n'"
    OUTPUT_VARIABLE OUTPUT_PATH
)

set(CMAKE_PREFIX_PATH ${OUTPUT_PATH})

include(FindThreads)
include(FindProtobuf)

file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/protos/*.proto")

protobuf_generate_cpp(PROTO_SRC PROTO_HEADER ${ProtoFiles})
# protobuf_generate_cpp(PROTO_SRC PROTO_HEADER ./protos/enum.proto)
# include PROTO_HEADER
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Search for libthrift* by best effort. If it is not found and brpc is
# compiled with thrift protocol enabled, a link error would be reported.
find_library(THRIFT_LIB NAMES thrift)
if (NOT THRIFT_LIB)
    set(THRIFT_LIB "")
endif()
find_library(THRIFTNB_LIB NAMES thriftnb)
if (NOT THRIFTNB_LIB)
    set(THRIFTNB_LIB "")
endif()

find_path(BRPC_INCLUDE_PATH NAMES brpc/server.h)
if(LINK_SO)
    find_library(BRPC_LIB NAMES brpc)
else()
    find_library(BRPC_LIB NAMES libbrpc.a brpc)
endif()
if((NOT BRPC_INCLUDE_PATH) OR (NOT BRPC_LIB))
    message(FATAL_ERROR "Fail to find brpc")
endif()
include_directories(${BRPC_INCLUDE_PATH})

find_path(GFLAGS_INCLUDE_PATH gflags/gflags.h)
find_library(GFLAGS_LIBRARY NAMES gflags libgflags)
if((NOT GFLAGS_INCLUDE_PATH) OR (NOT GFLAGS_LIBRARY))
    message(FATAL_ERROR "Fail to find gflags")
endif()
include_directories(${GFLAGS_INCLUDE_PATH})

execute_process(
    COMMAND bash -c "grep \"namespace [_A-Za-z0-9]\\+ {\" ${GFLAGS_INCLUDE_PATH}/gflags/gflags_declare.h | head -1 | awk '{print $2}' | tr -d '\n'"
    OUTPUT_VARIABLE GFLAGS_NS
)
if(${GFLAGS_NS} STREQUAL "GFLAGS_NAMESPACE")
    execute_process(
        COMMAND bash -c "grep \"#define GFLAGS_NAMESPACE [_A-Za-z0-9]\\+\" ${GFLAGS_INCLUDE_PATH}/gflags/gflags_declare.h | head -1 | awk '{print $3}' | tr -d '\n'"
        OUTPUT_VARIABLE GFLAGS_NS
    )
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    include(CheckFunctionExists)
    CHECK_FUNCTION_EXISTS(clock_gettime HAVE_CLOCK_GETTIME)
    if(NOT HAVE_CLOCK_GETTIME)
        set(DEFINE_CLOCK_GETTIME "-DNO_CLOCK_GETTIME_IN_MAC")
    endif()
endif()

set(CMAKE_CPP_FLAGS "${DEFINE_CLOCK_GETTIME} -DGFLAGS_NS=${GFLAGS_NS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CPP_FLAGS} -DNDEBUG -O0 -D__const__=__unused__ -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer")

if(CMAKE_VERSION VERSION_LESS "3.1.3")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
else()
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

find_path(LEVELDB_INCLUDE_PATH NAMES leveldb/db.h)
find_library(LEVELDB_LIB NAMES leveldb)
if ((NOT LEVELDB_INCLUDE_PATH) OR (NOT LEVELDB_LIB))
    message(FATAL_ERROR "Fail to find leveldb")
endif()
include_directories(${LEVELDB_INCLUDE_PATH})

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(OPENSSL_ROOT_DIR
        "/usr/local/opt/openssl"    # Homebrew installed OpenSSL
        )
endif()

find_package(OpenSSL)
include_directories(${OPENSSL_INCLUDE_DIR})

set(DYNAMIC_LIB
    ${CMAKE_THREAD_LIBS_INIT}
    ${GFLAGS_LIBRARY}
    ${PROTOBUF_LIBRARIES}
    ${LEVELDB_LIB}
    ${OPENSSL_CRYPTO_LIBRARY}
    ${OPENSSL_SSL_LIBRARY}
    ${THRIFT_LIB}
    ${THRIFTNB_LIB}
    dl
    )

file(GLOB_RECURSE SOURCES
    ./cpp/*.h
    ./cpp/*.cc
)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(DYNAMIC_LIB ${DYNAMIC_LIB}
        pthread
        "-framework CoreFoundation"
        "-framework CoreGraphics"
        "-framework CoreData"
        "-framework CoreText"
        "-framework Security"
        "-framework Foundation"
        "-Wl,-U,_MallocExtension_ReleaseFreeMemory"
        "-Wl,-U,_ProfilerStart"
        "-Wl,-U,_ProfilerStop")
endif()

add_executable(raft_client ./examples/raft_client.cc ${PROTO_SRC} ${PROTO_HEADER} ${SOURCES})
add_executable(raft_server ./examples/raft_server.cc ${PROTO_SRC} ${PROTO_HEADER} ${SOURCES})

target_link_libraries(raft_client ${BRPC_LIB} ${DYNAMIC_LIB})
target_link_libraries(raft_server ${BRPC_LIB} ${DYNAMIC_LIB})

enable_testing()
add_subdirectory(third_party/googletest-release-1.10.0)
include_directories(third_party/googletest-release-1.10.0/googletest/include)

find_path(GTEST_HEADER NAMES gtest/gtest.h)
find_library(GTEST_LIB NAMES gtest)
find_library(GTEST_MAIN_LIB NAMES gtest_main)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUNIT_TEST")

set(GTEST_LIB gtest gtest_main)

file(GLOB TEST_BRAFT_SRCS "test/test_*.cc")
foreach(BRAFT_UT ${TEST_BRAFT_SRCS})
    get_filename_component(BRAFT_UT_WE ${BRAFT_UT} NAME_WE)
    add_executable(${BRAFT_UT_WE} ${BRAFT_UT} 
                    ${PROTO_SRC} ${PROTO_HEADER} ${SOURCES})
    target_link_libraries(${BRAFT_UT_WE} ${GTEST_LIB}
        #"-Xlinker \"-(\""
                          ${BRPC_LIB} ${DYNAMIC_LIB}
                          #                  "-Xlinker \"-)\""
                          )
    add_test(NAME ${BRAFT_UT_WE} COMMAND ${BRAFT_UT_WE})                      
endforeach()

# add_executable(test_main test/tests_main.cc ${PROTO_SRC} ${PROTO_HEADER} ${SOURCES})
# target_link_libraries(test_main gtest gtest_main ${BRPC_LIB} ${DYNAMIC_LIB})

# add_test(NAME test_main COMMAND test_main)

# add_executable(test_log test/test_log.cc ${PROTO_SRC} ${PROTO_HEADER} ${SOURCES})
# target_link_libraries(test_log gtest gtest_main ${BRPC_LIB} ${DYNAMIC_LIB})

# add_test(NAME test_log COMMAND test_log)