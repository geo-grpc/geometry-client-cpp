FROM ubuntu:16.04
#FROM debian:stretch-slim

MAINTAINER David Raleigh <david@echoparklabs.io>

RUN DEBIAN_FRONTEND=noninteractive apt update && \
    apt-get install -y git \
    build-essential \
    autoconf \
    libtool \
    pkg-config \
    cmake && \
    rm -rf /var/lib/apt

WORKDIR /opt/src
RUN git clone https://github.com/google/googletest --branch release-1.7.0 --depth 1
WORKDIR /opt/src/googletest/build
RUN cmake ../ && \
    make && \
    cp -a /opt/src/googletest/include/gtest/ /usr/include && \
    cp -a /opt/src/googletest/build/libgtest.a /opt/src/googletest/build/libgtest_main.a /usr/lib/ && \
    ldconfig -v


ENV GRPC_RELEASE_TAG v1.9.1
#RUN git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc && \
WORKDIR /opt/src
RUN git clone -b ${GRPC_RELEASE_TAG} https://github.com/grpc/grpc && \
    cd grpc && \
    git submodule update --init && \
    make && \
    make install && \
    ldconfig

RUN cd /opt/src/grpc/third_party/protobuf && \
    make && \
    make install && \
    ldconfig

WORKDIR /opt/src/geometry-client-cpp
COPY ./ ./

RUN protoc -I ./protos --grpc_out=./geometry --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./protos/geometry_operators.proto
RUN protoc -I ./protos --cpp_out=./geometry ./protos/geometry_operators.proto
WORKDIR /opt/src/geometry-client-cpp/build
RUN cmake .. && \
    make

WORKDIR /opt/src/geometry-client-cpp
