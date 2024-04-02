cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(Descartes-Third)
include(ExternalProject)

set(INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
message("install dir: " ${INSTALL_DIR})

set(OPENSSL_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/openssl)
ExternalProject_Add(openssl
    URL               "https://www.openssl.org/source/old/1.1.1/openssl-1.1.1i.tar.gz"
    CONFIGURE_COMMAND <SOURCE_DIR>/config no-shared no-tests no-ui-console --prefix=${INSTALL_DIR} --openssldir=ssl --release
    BUILD_COMMAND     make -j build_libs
    INSTALL_COMMAND   make LIBDIR=lib install_dev
    SOURCE_DIR        ${OPENSSL_ROOT_DIR}
    LOG_DOWNLOAD      ON
    LOG_CONFIGURE     ON
    LOG_BUILD         ON
    LOG_INSTALL       ON
)

set(PROTOBUF_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/protobuf)
ExternalProject_Add(protobuf
    GIT_REPOSITORY    git@github.com:protocolbuffers/protobuf.git
    GIT_TAG           v3.20.0
    GIT_SHALLOW       true
    SOURCE_DIR        ${PROTOBUF_ROOT_DIR}
    CONFIGURE_COMMAND cd ${PROTOBUF_ROOT_DIR} && ./autogen.sh && ./configure --prefix=${INSTALL_DIR}
    BUILD_COMMAND     cd ${PROTOBUF_ROOT_DIR} && make -j8
    INSTALL_COMMAND   cd ${PROTOBUF_ROOT_DIR} && make install
)

set(GFLAGS_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/gflags)
ExternalProject_Add(gflags
    GIT_REPOSITORY    git@github.com:gflags/gflags.git
    GIT_TAG           master 
    GIT_SHALLOW       true
    SOURCE_DIR        ${GFLAGS_ROOT_DIR}
    CONFIGURE_COMMAND cmake ${GFLAGS_ROOT_DIR} -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DGFLAGS_BUILD_SHARED_LIBS=ON
    BUILD_COMMAND     make -j8
    INSTALL_COMMAND   make install
)

set(GLOG_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/glog)
ExternalProject_Add(glog
    GIT_REPOSITORY    git@github.com:google/glog.git
    GIT_TAG           v0.6.0
    GIT_SHALLOW       true
    SOURCE_DIR        ${GLOG_ROOT_DIR}
    CONFIGURE_COMMAND cmake ${GLOG_ROOT_DIR} -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
                              -DBUILD_SHARED_LIBS=ON
                              -DCMAKE_BUILD_TYPE=Release
                              -DWITH_GTEST=OFF
                              -DWITH_GFLAGS=ON
    BUILD_COMMAND     make -j8
    INSTALL_COMMAND   make install
)
add_dependencies(glog gflags)

set(LEVELDB_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/leveldb)
ExternalProject_Add(leveldb
    GIT_REPOSITORY    git@github.com:google/leveldb.git
    GIT_TAG           main
    GIT_SHALLOW       true
    SOURCE_DIR        ${LEVELDB_ROOT_DIR}
    CONFIGURE_COMMAND  cmake ${LEVELDB_ROOT_DIR} -DCMAKE_BUILD_TYPE=Release 
                                -DLEVELDB_BUILD_TESTS=OFF 
                                -DLEVELDB_BUILD_BENCHMARKS=OFF
                                -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
    BUILD_COMMAND      make -j8
    INSTALL_COMMAND    make install
)

set(BRPC_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/brpc)
ExternalProject_Add(brpc
    GIT_REPOSITORY    git@github.com:apache/brpc.git
    GIT_TAG           1.6.0
    GIT_SHALLOW       true
    SOURCE_DIR        ${BRPC_ROOT_DIR}
    CONFIGURE_COMMAND cmake ${BRPC_ROOT_DIR} -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
                              -DCMAKE_BUILD_TYPE=Release 
                              -DDOWNLOAD_GTEST=OFF
                              -DWITH_DEBUG_SYMBOLS=OFF
                              -DWITH_GLOG=ON
                              -DBUILD_BRPC_TOOLS=OFF
    BUILD_COMMAND     make -j8
    INSTALL_COMMAND   make install
)
add_dependencies(brpc openssl)
add_dependencies(brpc protobuf)
add_dependencies(brpc gflags)
add_dependencies(brpc glog)
add_dependencies(brpc leveldb)
