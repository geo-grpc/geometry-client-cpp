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

set(PROTOS_PROTOBUF ${CMAKE_SOURCE_DIR}/proto/epl/protobuf/geometry.proto)
set(PROTOS_GRPC ${CMAKE_SOURCE_DIR}/proto/epl/protobuf/geometry_service.proto)

set(PROTO_SRC_DIR ${CMAKE_SOURCE_DIR}/geometry/src)
file(MAKE_DIRECTORY ${PROTO_SRC_DIR})
file(MAKE_DIRECTORY ${PROTO_SRC_DIR}/epl)
file(MAKE_DIRECTORY ${PROTO_SRC_DIR}/epl/protobuf)

set(PROTOBUF_GENERATE_CPP_APPEND_PATH OFF)
set(PROTOBUF_IMPORT_DIRS ${CMAKE_SOURCE_DIR}/proto)
set(protobuf_generate_PROTOC_OUT_DIR ${PROTO_SRC_DIR}/epl/protobuf)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC_DIR} ${PROTOS_PROTOBUF})
MESSAGE(STATUS "PROTO_HDRS ${PROTO_HDRS}")

set(protobuf_generate_PROTOC_OUT_DIR ${PROTO_SRC_DIR}/epl/protobuf)
set(PROTOBUF_IMPORT_DIRS ${CMAKE_SOURCE_DIR}/proto)
protobuf_generate_cpp(PROTO_SRCS_2 PROTO_HDRS_2 ${PROTO_SRC_DIR} ${PROTOS_GRPC})
MESSAGE(STATUS "PROTO_HDRS_2 ${PROTO_HDRS_2}")

set(protobuf_generate_PROTOC_OUT_DIR ${PROTO_SRC_DIR}/epl/protobuf)
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_SRC_DIR} ${PROTOS_GRPC})

include_directories(${PROTO_SRC_DIR} /usr/local/include/grpc++)
add_custom_target(run ALL
        DEPENDS ${PROTO_HDRS}
        DEPENDS ${PROTO_HDRS_2}
        DEPENDS ${GRPC_HDRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")


# TODO this might change depending on system
set(SOURCE_FILES
        ${PROTO_HDRS}
        ${PROTO_SRCS}
        ${PROTO_HDRS_2}
        ${PROTO_SRCS_2}
        ${GRPC_HDRS}
        ${GRPC_SRCS})

add_library(geometry_lib
        ${SOURCE_FILES})