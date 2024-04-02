add_executable(main main.cpp service.cpp)

find_package(Protobuf REQUIRED)
file(GLOB PROTO_FILES  *.proto)
add_library(server-proto STATIC ${PROTO_FILES})
protobuf_generate(
    TARGET server-proto
    LANGUAGE cpp
    IMPORT_DIRS ${PROTOBUF_IMPORT_DIRS}
    PROTOC_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}
)
target_include_directories(server-proto PUBLIC ${PROTOBUF_INCLUDE_DIR})
target_link_libraries(server-proto PUBLIC ${PROTOBUF_LIBRARIES})

target_link_directories(main PRIVATE ${INSTALL_DIR}/lib ${INSTALL_DIR}/lib64 ${DESCARTES_LIB_DIR})
target_include_directories(main PRIVATE ${INSTALL_DIR}/include ${DESCARTES_INCLUDE} ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(main PRIVATE "-Wl,--start-group" pthread crypto ssl dl z gflags glog leveldb brpc descartes server-proto "-Wl,--end-group")
set_property(TARGET main PROPERTY OUTPUT_NAME descartes_server)

install(TARGETS main RUNTIME)