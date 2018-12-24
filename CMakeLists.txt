# Version 3.9 required because of C++17 / LTO support.
cmake_minimum_required(VERSION 3.9)
project(cbag)

# Boost Spirit X3 requires C++14
# std::variant requires C++17
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
# check for link time optimization
include(CheckIPOSupported)

check_ipo_supported(RESULT ipo_result)
if(ipo_result AND CMAKE_BUILD_TYPE MATCHES Release)
  message("Enabling interprocedural optimization")
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message("Interprocedural optimization disabled")
endif()

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# Include Boost as an imported target
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS system)

message("Boost include directory: " ${Boost_INCLUDE_DIRS})
message("Boost library directory: " ${Boost_LIBRARY_DIRS})
message("Boost libraries: " ${Boost_LIBRARIES})

# Include fmt library
# use add_subdirectory, as they do not support find_package
# set to build static library and enable fPIC
add_subdirectory(fmt EXCLUDE_FROM_ALL)

# Include spdlog
find_package(spdlog CONFIG REQUIRED)
# Include threads for spdlog
# find_package(Threads REQUIRED)

# Include yaml-cpp
add_subdirectory(yaml-cpp EXCLUDE_FROM_ALL)

# Include Catch2
find_package(Catch2 REQUIRED)

# set variable storing binary/library location
message("install prefix: " ${CMAKE_INSTALL_PREFIX})
set(CBAG_BINARY_DIR ${CMAKE_INSTALL_PREFIX}/cbag/bin)
set(CBAG_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/cbag/lib)
message("install rpath: " ${CMAKE_INSTALL_RPATH})

set(SRC_FILES_LIB_CBAG
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/common/box_t.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/common/box_t_util.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/common/transformation_util.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/common/via_param.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/gdsii/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/layout/blockage.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/layout/boundary.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/layout/cellview.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/layout/geo_index.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/layout/geo_instance.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/layout/geo_iterator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/layout/geo_object.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/layout/geometry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/layout/instance.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/layout/path_util.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/layout/pin.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/layout/tech.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/layout/vector45.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/layout/via.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/logging/logging.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/netlist/cdl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/netlist/core.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/netlist/lstream.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/netlist/netlist.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/netlist/verilog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/schematic/arc.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/schematic/cellview.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/schematic/cellview_info.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/schematic/donut.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/schematic/ellipse.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/schematic/eval_text.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/schematic/instance.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/schematic/line.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/schematic/path.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/schematic/pin_figure.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/schematic/pin_object.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/schematic/polygon.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/schematic/rectangle.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/schematic/shape_base.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/schematic/term_attr.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/schematic/text_base.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/schematic/text_t.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/spirit/ast.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/spirit/name.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/spirit/name_unit.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/spirit/range.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/util/io.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/util/name_convert.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/yaml/box_t.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/yaml/transformation.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/yaml/datatypes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/yaml/shapes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/yaml/figures.cpp
  )

set(SRC_FILES_LIB_CBAG_OA
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/oa/oa_database.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/oa/oa_read.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/oa/oa_read_lib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/oa/oa_util.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbag/oa/oa_write.cpp
  )


if(DEFINED ENV{OA_LINK_DIR})
message("OA include directory: " $ENV{OA_INCLUDE_DIR})
message("OA link directory: " $ENV{OA_LINK_DIR})

# setup global include directories and link libraries
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
  ${Boost_INCLUDE_DIRS}
  $ENV{OA_INCLUDE_DIR}
  )
link_directories($ENV{OA_LINK_DIR})

add_library(cbag ${SRC_FILES_LIB_CBAG} ${SRC_FILES_LIB_CBAG_OA})

# shared library dependencies
target_link_libraries(cbag
  PUBLIC
  fmt
  oaBase
  oaCommon
  oaPlugIn
  PRIVATE
  stdc++fs
  ${Boost_LIBRARIES}
  yaml-cpp
  oaDM
  oaDMFileSysBase
  oaTech
  oaDesign
  ${CMAKE_DL_LIBS}
  )

else()
message("WARNING: building without OpenAccess support.")

# setup global include directories and link libraries
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
  ${Boost_INCLUDE_DIRS}
  )

add_library(cbag ${SRC_FILES_LIB_CBAG})

# shared library dependencies
target_link_libraries(cbag
  PUBLIC
  fmt
  PRIVATE
  stdc++fs
  ${Boost_LIBRARIES}
  yaml-cpp
  )

endif()

# add utility binaries
add_subdirectory(util EXCLUDE_FROM_ALL)

# add tests binaries
add_subdirectory(tests EXCLUDE_FROM_ALL)
