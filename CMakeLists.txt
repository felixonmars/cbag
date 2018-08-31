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
  ${CMAKE_SOURCE_DIR}/src/cbag/spirit/ast.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/spirit/range.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/spirit/name_unit.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/spirit/name.cpp
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
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/blockage.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/boundary.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/cellview.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/blockage.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/geometry.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/instance.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/point_t.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/polygon.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/polygon45.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/polygon45_view.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/polygon90.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/polygon90_view.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/polygon_view.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/pt_vector.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/rectangle.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/transformation.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/union_view.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/vector45.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/layout/via.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/netlist/name_convert.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/netlist/netlist.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/netlist/cdl.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/netlist/verilog.cpp
  ${CMAKE_SOURCE_DIR}/src/cbag/cbag.cpp
  )
add_library(cbag SHARED ${SRC_FILES_LIB_CBAG})

# shared library dependencies
target_link_libraries(cbag
  PUBLIC
  # for primitives YAML serialization
  oaBase
  oaCommon
  oaPlugIn
  PRIVATE
  # for primitives YAML serialization
  fmt-header-only
  # for Spirit X3 error code
  boost_system
  # multithreading for spdlog
  # Threads::Threads
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
  yaml-cpp
  boost_system
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

add_executable(netmap_sample
  ${CMAKE_SOURCE_DIR}/test/netmap_sample.cpp
  )

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(netmap_sample PROPERTIES
  INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
  )

# shared library dependencies
target_link_libraries(netmap_sample
  cbag
  cbagyaml
  yaml-cpp
  boost_system
  )

add_executable(test_polygon
  ${CMAKE_SOURCE_DIR}/test/test_polygon.cpp
  )

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(test_polygon PROPERTIES
  INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
  )

target_link_libraries(test_polygon
  cbag
  )

add_executable(test_casting
  ${CMAKE_SOURCE_DIR}/test/test_casting.cpp
  )

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(test_casting PROPERTIES
  INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
  )


install(TARGETS
  cbag
  cbagyaml
  cbagoa
#  test_oa
#  test_spirit
#  read_prop
#  test_yaml
#  netmap_sample
  test_polygon
#  test_casting
  RUNTIME DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin
  LIBRARY DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/lib
  )
