cmake_minimum_required(VERSION 3.10)
project(cxbase)

# set CXX options
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -g -Wall -Wno-unused-variable")

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# Include Boost as an imported target
find_package(Boost REQUIRED)
add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

include_directories(include)

set(SRC_FILES_LIB
        src/templates.cpp
        src/objects.cpp include/cxbase/database.h)
add_library(cxbase SHARED ${SRC_FILES_LIB})

set(SRC_FILES_TEST
        test/test_template.cpp
        )
add_executable(test_template ${SRC_FILES_TEST})
target_link_libraries(test_template cxbase)
