project(oic)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror=sign-compare")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

FIND_LIBRARY(COAP PATHS /home/pawwik/dev/libcoap-build/ NAMES coap)
INCLUDE_DIRECTORIES(/home/pawwik/dev/libcoap/)
INCLUDE_DIRECTORIES(/home/pawwik/dev/std/)

add_library(oic SHARED ${SRC_LIST})



