cmake_minimum_required(VERSION 3.10)
project(cxbase)

set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -g -Wall -Wno-unused-variable")

find_package(Boost 1.67.0)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find boost!")
else()
    include_directories(${Boost_INCLUDE_DIR})
endif()

add_library(cxbase SHARED include/cxbase/objects.h include/cxbase/templates.h)

add_executable (test_main test/test.cpp include/cxbase/objects.h include/cxbase/templates.h)
