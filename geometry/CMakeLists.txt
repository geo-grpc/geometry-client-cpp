cmake_minimum_required(VERSION 3.7)
project(geometry_lib)
set(CMAKE_VERBOSE_MAKEFILE ON)
# Cmake find modules
# GRPC and Protocol Buffers libraries location


if (APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/Users/davidraleigh/code/google/grpc" "/Users/davidraleigh/code/google/grpc/third_party/protobuf")
else()
    list(APPEND CMAKE_PREFIX_PATH "/opt/src/grpc" "/opt/src/grpc/third_party/protobuf")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")

find_package(Protobuf REQUIRED)
find_package(GRPC REQUIRED)

set(PROTOS_PROTOBUF ${CMAKE_SOURCE_DIR}/proto/geometry.proto)
set(PROTOS_GRPC ${CMAKE_SOURCE_DIR}/proto/geometry_operators.proto)

set(PROTO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(MAKE_DIRECTORY ${PROTO_SRC_DIR})

set(PROTOBUF_IMPORT_DIRS ${CMAKE_SOURCE_DIR}/proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC_DIR} ${PROTOS_GRPC} ${PROTOS_PROTOBUF})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_SRC_DIR} ${PROTOS_GRPC})

include_directories(${PROTO_SRC_DIR} /usr/local/include/grpc++)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# TODO this might change depending on system
set(SOURCE_FILES
        ${PROTO_HDRS}
        ${PROTO_SRCS}
        ${GRPC_HDRS}
        ${GRPC_SRCS})

add_library(geometry_lib
        ${SOURCE_FILES})