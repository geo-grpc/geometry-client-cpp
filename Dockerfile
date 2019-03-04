FROM debian:sid

MAINTAINER David Raleigh <david@echoparklabs.io>

RUN DEBIAN_FRONTEND=noninteractive apt update && \
    apt-get install -y build-essential \
    autoconf \
    pkg-config \
    libprotobuf-dev \
    libgrpc++-dev \
    protobuf-compiler-grpc \
    libgtest-dev \
    cmake && \
    rm -rf /var/lib/apt

RUN cd /usr/src/gtest && \
    cmake CMakeLists.txt && \
    make && \
    cp *.a /usr/lib

WORKDIR /opt/src/geometry-client-cpp
COPY ./ ./

WORKDIR /opt/src/geometry-client-cpp/build
RUN cmake .. && \
    make

