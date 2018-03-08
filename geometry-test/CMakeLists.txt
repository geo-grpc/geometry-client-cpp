# https://github.com/dmonopoly/gtest-cmake-example/blob/master/CMakeLists.txt
# http://www.kaizou.org/2014/11/gtest-cmake/
# https://stackoverflow.com/questions/8507723/how-to-start-working-with-gtest-and-cmake
cmake_minimum_required(VERSION 3.5)

# Cmake find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")

find_package(Protobuf REQUIRED)
find_package(GRPC REQUIRED)

project(unitTest C CXX)
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/geometry
        /usr/local/include/grpc++)

set(SOURCE_FILES ./main.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_executable(unitTest ${SOURCE_FILES})


target_link_libraries(unitTest
        gRPC::grpc++_reflection
        protobuf::libprotobuf
        geometry_lib
        ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})

