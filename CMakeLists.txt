# Version 3.8 required because of C++17 support.
cmake_minimum_required(VERSION 3.8)
project(cbag)

# Boost Spirit X3 requires C++14
# __has_include directive requires C++17
# std::variant requires C++17
# map::extract (use of node handles) requires C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# control compiler warnings
add_compile_options(-fexceptions)
add_compile_options(-g)
add_compile_options(-Wall)
add_compile_options(-Wno-delete-non-virtual-dtor)
# add_compile_options(-Wno-logical-op-parentheses)
# add_compile_options(-Wno-new-returns-null)
# set optimzation level for release
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
# generate compilation commands file for emacs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# prefer pthreads
set(THREADS_PREFER_PTHREAD_FLAG ON)

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
add_subdirectory(fmt EXCLUDE_FROM_ALL)

# Include spdlog
find_package(spdlog CONFIG REQUIRED)
# Include threads for spdlog
# find_package(Threads REQUIRED)

# Include yaml-cpp
find_package(yaml-cpp)
message("yaml-cpp include directory: " ${YAML_CPP_INCLUDE_DIR})
message("yaml-cpp library: " ${YAML_CPP_LIBRARIES})

# Include Catch2
find_package(Catch2 REQUIRED)

# setup global include directories and link libraries
include_directories(${CMAKE_SOURCE_DIR}/include
  $ENV{OA_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${YAML_CPP_INCLUDE_DIR}
  )

# setup link library path.  Must call before defining target.
link_directories($ENV{OA_LINK_DIR})

# setup install location
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}" CACHE PATH "CMake install prefix" FORCE)
endif()

message("install prefix: " ${CMAKE_INSTALL_PREFIX})

# set variable storing binary/library location
set(CBAG_BINARY_DIR ${CMAKE_INSTALL_PREFIX}/cbag/bin)
set(CBAG_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/cbag/lib)

# setup RPATH
set(CMAKE_INSTALL_RPATH "${CBAG_LIBRARY_DIR}:${Boost_LIBRARY_DIRS}")

message("install rpath: " ${CMAKE_INSTALL_RPATH})

set(SRC_FILES_LIB_CBAG
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/blockage.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/boundary.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/cellview.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/geo_index.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/geo_instance.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/geo_iterator.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/geo_object.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/geometry.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/instance.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/pin.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/rectangle.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/tech.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/transformation.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/vector45.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/via.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/netlist/name_convert.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/netlist/netlist.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/netlist/cdl.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/netlist/verilog.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/schematic/arc.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/schematic/cellview.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/schematic/cellview_info.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/schematic/donut.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/schematic/ellipse.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/schematic/eval_text.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/schematic/instance.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/schematic/line.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/schematic/path.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/schematic/pin_figure.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/schematic/pin_object.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/schematic/polygon.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/schematic/rectangle.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/schematic/shape_base.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/schematic/term_attr.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/schematic/text_base.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/schematic/text_t.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/spirit/ast.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/spirit/range.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/spirit/name_unit.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/spirit/name.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/cbag.cpp
  )
add_library(cbag SHARED ${SRC_FILES_LIB_CBAG})

# shared library dependencies
target_link_libraries(cbag
  PUBLIC
  oaTech
  oaBase
  oaCommon
  oaPlugIn
  # for Spirit X3 error code
  boost_system
  # for reading tech files
  yaml-cpp
  PRIVATE
  fmt-header-only
  m
  ${CMAKE_DL_LIBS}
  )

set(SRC_FILES_LIB_CBAGYAML
  ${CMAKE_SOURCE_DIR}/src/cbagyaml/box_t.cpp
  ${CMAKE_SOURCE_DIR}/src/cbagyaml/point.cpp
  ${CMAKE_SOURCE_DIR}/src/cbagyaml/point_array.cpp
  ${CMAKE_SOURCE_DIR}/src/cbagyaml/transform.cpp
  ${CMAKE_SOURCE_DIR}/src/cbagyaml/datatypes.cpp
  ${CMAKE_SOURCE_DIR}/src/cbagyaml/shapes.cpp
  ${CMAKE_SOURCE_DIR}/src/cbagyaml/figures.cpp
  ${CMAKE_SOURCE_DIR}/src/cbagyaml/cbagyaml.cpp
  )
add_library(cbagyaml SHARED ${SRC_FILES_LIB_CBAGYAML})

# shared library dependencies
target_link_libraries(cbagyaml
  PUBLIC
  cbag
  # for yaml file path manipulations
  boost_filesystem
  yaml-cpp
  PRIVATE
  boost_system
  fmt-header-only
  )


set(SRC_FILES_LIB_CBAGOA
  ${CMAKE_SOURCE_DIR}/src/cbagoa/oa_reader.cpp
  ${CMAKE_SOURCE_DIR}/src/cbagoa/oa_writer.cpp
  ${CMAKE_SOURCE_DIR}/src/cbagoa/oa_database.cpp
  )
add_library(cbagoa SHARED ${SRC_FILES_LIB_CBAGOA})

# shared library dependencies
target_link_libraries(cbagoa
  PUBLIC
  cbag
  cbagyaml
  boost_filesystem
  boost_system
  yaml-cpp
  PRIVATE
  oaDM
  oaDMFileSysBase
  oaTech
  oaDesign
  fmt-header-only
  m
  ${CMAKE_DL_LIBS}
  )

# add utility binaries
add_subdirectory(util)

# add test binaries
add_subdirectory(test)

install(TARGETS
  cbag
  cbagyaml
  cbagoa
  RUNTIME DESTINATION ${CBAG_BINARY_DIR}
  LIBRARY DESTINATION ${CBAG_LIBRARY_DIR}
  )
