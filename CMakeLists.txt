# Version 3.8 required because of C++17 support.
cmake_minimum_required(VERSION 3.8)
project(cbag)

# Boost Spirit X3 requires C++14
# __has_include directive requires C++17
# std::filesystem requires C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# control compiler warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -g -Wall \
-Wno-unknown-pragmas -Wno-delete-non-virtual-dtor")
# set optimzation level for release
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# make sure linker raise errors if shared library has undefined symbols
# this makes it a lot easier to debug
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")
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

# include yaml-cpp
find_package(yaml-cpp)
message("yaml-cpp include directory: " ${YAML_CPP_INCLUDE_DIR})
message("yaml-cpp library: " ${YAML_CPP_LIBRARIES})

# setup global include directories and link libraries
include_directories(${CMAKE_SOURCE_DIR}/include
        $ENV{OA_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${YAML_CPP_INCLUDE_DIR}
        )

# setup link library path.  Must call before defining target.
link_directories($ENV{OA_LINK_DIR})

set(SRC_FILES_LIB_CBAG
        ${CMAKE_SOURCE_DIR}/easyloggingpp/src/easylogging++.cc
        ${CMAKE_SOURCE_DIR}/src/cbag/spirit/ast.cpp
        ${CMAKE_SOURCE_DIR}/src/cbag/spirit/range.cpp
        ${CMAKE_SOURCE_DIR}/src/cbag/spirit/name_unit.cpp
        ${CMAKE_SOURCE_DIR}/src/cbag/spirit/name.cpp
        ${CMAKE_SOURCE_DIR}/src/cbag/database/yaml_primitives.cpp
        ${CMAKE_SOURCE_DIR}/src/cbag/database/yaml_datatypes.cpp
        ${CMAKE_SOURCE_DIR}/src/cbag/database/yaml_figures.cpp
        ${CMAKE_SOURCE_DIR}/src/cbag/database/cellviews.cpp
        )
add_library(cbag SHARED ${SRC_FILES_LIB_CBAG})

# shared library dependencies
target_link_libraries(cbag
        fmt-header-only
        # for Spirit X3 error code
        boost_system
        # for primitives YAML serialization
        yaml-cpp
        # for primitives YAML serialization
        oaBase
        oaCommon
        oaPlugIn
        m
        ${CMAKE_DL_LIBS}
        )

set(SRC_FILES_LIB_CBAGOA
        ${CMAKE_SOURCE_DIR}/src/cbagoa/name.cpp
        ${CMAKE_SOURCE_DIR}/src/cbagoa/read_oa.cpp
        ${CMAKE_SOURCE_DIR}/src/cbagoa/write_oa.cpp
        ${CMAKE_SOURCE_DIR}/src/cbagoa/database.cpp
        ${CMAKE_SOURCE_DIR}/easyloggingpp/src/easylogging++.cc
        )
add_library(cbagoa SHARED ${SRC_FILES_LIB_CBAGOA})

# shared library dependencies
target_link_libraries(cbagoa
        cbag
        oaDM
        oaDMFileSysBase
        oaTech
        oaDesign
        boost_filesystem
        )

set(SRC_FILES_OA
        ${CMAKE_SOURCE_DIR}/test/test_oa.cpp
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
        )

add_executable(test_spirit
        ${CMAKE_SOURCE_DIR}/test/test_spirit.cpp
        )

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(test_spirit PROPERTIES
        INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
        )

target_link_libraries(test_spirit
        cbag
        )


add_executable(read_prop
        ${CMAKE_SOURCE_DIR}/test/read_prop.cpp
        )

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(read_prop PROPERTIES
        INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
        )

# shared library dependencies
target_link_libraries(read_prop
        oaCommon
        oaBase
        oaPlugIn
        oaDM
        oaDMFileSysBase
        oaTech
        oaDesign
        ${CMAKE_DL_LIBS}
        )

add_executable(test_yaml
        ${CMAKE_SOURCE_DIR}/test/test_yaml.cpp
        )

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(test_yaml PROPERTIES
        INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
        )

# shared library dependencies
target_link_libraries(test_yaml
        fmt-header-only
        yaml-cpp
        )


install(TARGETS test_oa test_spirit cbag cbagoa read_prop test_yaml
        RUNTIME DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin
        LIBRARY DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/lib)
