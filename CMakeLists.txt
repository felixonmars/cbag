cmake_minimum_required(VERSION 3.7)
project(cbag)

# build with C++11
set(CMAKE_CXX_STANDARD 11)
# control compiler warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -g -Wall \
-Wno-unknown-pragmas -Wno-delete-non-virtual-dtor")
# make executable in build tree and install location have same rpaths
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
# add rpaths to the final install executable
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# Include Boost as an imported target
find_package(Boost REQUIRED)
add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

# NOTE: apparently, there's no good way in cmake now to automatically add RPATH of
# imported libraries to executable.  Since yaml-cpp is an imported library, we
# just get the library file location and add to RPATH ourselves.

# Find yaml-cpp package
find_package(yaml-cpp REQUIRED)
# Find yaml-cpp .so file location, extract directory, then add to RPATH
get_target_property(YAML_LIB_PATH yaml-cpp LOCATION)
get_filename_component(YAML_LIB_DIR ${YAML_LIB_PATH} DIRECTORY)

set(CMAKE_INSTALL_RPATH ${YAML_LIB_DIR})

message("yaml-cpp library file: " ${YAML_LIB_PATH})
message("yaml-cpp library directory: " ${YAML_LIB_DIR})
message("yaml-cpp include directory: " ${YAML_CPP_INCLUDE_DIR})

# setup global include directories and link libraries
include_directories(${CMAKE_SOURCE_DIR}/include
        $ENV{OA_INCLUDE_DIR}
        ${Boost_INCLUDE_DIR}
        ${YAML_CPP_INCLUDE_DIR}
        )
link_libraries( yaml-cpp )

set(SRC_FILES_LIB_CBAG
        src/cbag/layout/templates.cpp
        src/cbag/layout/objects.cpp
        src/cbag/schematic/objects.cpp src/cbag/common.cpp)
add_library(cbag SHARED ${SRC_FILES_LIB_CBAG})

# setup link library path.  Must call before defining target.
link_directories($ENV{OA_LINK_DIR})

set(SRC_FILES_LIB_CBAGOA
        src/cbagoa/database.cpp
        )
add_library(cbagoa SHARED ${SRC_FILES_LIB_CBAGOA})

# shared library dependencies
target_link_libraries(cbagoa
        cbag
        )

set(SRC_FILES_OA
        test/test_oa.cpp
        )
add_executable(test_oa ${SRC_FILES_OA})

# shared library dependencies
target_link_libraries(test_oa
        cbagoa
        oaCommon
        oaBase
        oaPlugIn
        oaDM
        oaTech
        oaDesign
        ${CMAKE_DL_LIBS}
        )

# append yaml-cpp library directory and build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(test_oa PROPERTIES
        INSTALL_RPATH "${YAML_LIB_DIR}:${CMAKE_CURRENT_BINARY_DIR}")