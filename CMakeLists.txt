# Version 3.8 required because of C++17 support.
cmake_minimum_required(VERSION 3.8)
project(cbag)

# Boost Spirit X3 requires C++14
# __has_include directive requires C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# control compiler warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -g -Wall \
-Wno-unknown-pragmas -Wno-delete-non-virtual-dtor")
# add rpaths to the final install executable
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# Include Boost as an imported target
find_package(Boost REQUIRED)
add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})

message("Boost include directory: " ${Boost_INCLUDE_DIRS})
message("Boost library directory: " ${Boost_LIBRARY_DIRS})

# Include fmt library
add_subdirectory(fmt)

# setup global include directories and link libraries
include_directories(${CMAKE_SOURCE_DIR}/include
        $ENV{OA_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/easyloggingpp/src
        )

# boost_system : for Spirit X3 error handling
# boost_serialization : for serialization
# spdlog::spdlog : the spdlog logging library
link_libraries(boost_system boost_serialization fmt-header-only)

set(SRC_FILES_LIB_CBAG
        src/cbag/spirit/ast.cpp
        src/cbag/spirit/range.cpp
        src/cbag/spirit/name_unit.cpp
        src/cbag/spirit/name.cpp
        )
add_library(cbag SHARED ${SRC_FILES_LIB_CBAG})

# setup link library path.  Must call before defining target.
link_directories($ENV{OA_LINK_DIR})

set(SRC_FILES_LIB_CBAGOA
        src/cbagoa/name.cpp
        src/cbagoa/read_oa.cpp
        src/cbagoa/write_oa.cpp
        src/cbagoa/database.cpp
        ${CMAKE_SOURCE_DIR}/easyloggingpp/src/easylogging++.cc
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

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(test_oa PROPERTIES
        INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
        )

# shared library dependencies
target_link_libraries(test_oa
        cbagoa
        oaCommon
        oaBase
        oaPlugIn
        oaDM
        oaDMFileSysBase
        oaTech
        oaDesign
        ${CMAKE_DL_LIBS}
        )

add_executable(test_spirit
        test/test_spirit.cpp
        )

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(test_spirit PROPERTIES
        INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
        )

target_link_libraries(test_spirit
        cbag
        )

install(TARGETS test_oa test_spirit cbag cbagoa
        RUNTIME DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin
        LIBRARY DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/lib)
