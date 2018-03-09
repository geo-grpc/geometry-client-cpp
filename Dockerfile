FROM us.gcr.io/echoparklabs/grpc-cpp:latest

MAINTAINER David Raleigh <david@echoparklabs.io>

RUN DEBIAN_FRONTEND=noninteractive apt update

WORKDIR /opt/src
RUN git clone https://github.com/google/googletest --branch release-1.7.0 --depth 1
WORKDIR /opt/src/googletest/build
RUN cmake ../ && \
    make && \
    cp -a /opt/src/googletest/include/gtest/ /usr/include && \
    cp -a /opt/src/googletest/build/libgtest.a /opt/src/googletest/build/libgtest_main.a /usr/lib/ && \
    ldconfig -v

WORKDIR /opt/src/geometry-client-cpp
COPY ./ ./

#RUN protoc -I ./protos --grpc_out=./geometry --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./protos/geometry_operators.proto
#RUN protoc -I ./protos --cpp_out=./geometry ./protos/geometry_operators.proto
WORKDIR /opt/src/geometry-client-cpp/build
RUN cmake .. && \
    make

