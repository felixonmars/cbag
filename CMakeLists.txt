cmake_minimum_required(VERSION 3.7)
project(cxbase)

# set CXX options
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -g -Wall \
-Wno-unknown-pragmas -Wno-delete-non-virtual-dtor")

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# Include Boost as an imported target
find_package(Boost REQUIRED)
add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

# Include yaml-cpp as an imported target
find_package(yaml-cpp REQUIRED)

# setup include libraries
# TODO: Why do I have to list Boost include here again?
include_directories(${CMAKE_SOURCE_DIR}/include
        $ENV{OA_INCLUDE_DIR}
        ${Boost_INCLUDE_DIR}
        )

# setup link library path.  Must call before defining target.
link_directories($ENV{OA_LINK_DIR})

set(SRC_FILES_LIB
        src/templates.cpp
        src/objects.cpp
        )
add_library(cxbase SHARED ${SRC_FILES_LIB})


set(SRC_FILES_OA
        test/test_oa.cpp
        src/bagoa.cpp
        )
add_executable(test_oa ${SRC_FILES_OA})

# shared library dependencies
target_link_libraries(test_oa
        yaml-cpp
        oaCommon
        oaBase
        oaPlugIn
        oaDM
        oaTech
        oaDesign
        ${CMAKE_DL_LIBS}
        )
