project(oic)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror=sign-compare")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

#set(CMAKE_SYSTEM_NAME  Android)
#set(CMAKE_SYSTEM_VERSION 1)
#set(CMAKE_C_COMPILER ../../ndk-toolchain_21/bin/arm-linux-androideabi-gcc)
#set(CMAKE_CXX_COMPILER ../../ndk-toolchain_21/bin/arm-linux-androideabi-g++)


FIND_LIBRARY(COAP PATHS ../libcoap/ NAMES coap)
INCLUDE_DIRECTORIES(../libcoap/)
INCLUDE_DIRECTORIES(../std/)

add_library(oic SHARED ${SRC_LIST})



