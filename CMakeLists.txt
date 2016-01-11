project(oic)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

FIND_LIBRARY(CBOR PATHS /home/pawwik/dev/cbor-cpp-build/ NAMES cborcpp)
INCLUDE_DIRECTORIES(/home/pawwik/dev/cbor-cpp/src/)

add_library(oic SHARED ${SRC_LIST})



