cmake_minimum_required(VERSION 3.7)
project(geometry_operators)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/geometry)

include_directories(${CMAKE_SOURCE_DIR}/geometry /usr/local/include/grpc++)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

target_link_libraries(geometry_lib)

enable_testing()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/geometry-test)
