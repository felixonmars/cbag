add_subdirectory(../Catch2 catch2)

add_executable(test_main
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/common/box_t.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/common/transformation.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/gdsii/gds_lookup.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/gdsii/io.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/gdsii/math.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/layout/cellview.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/layout/path.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/layout/grid.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/layout/tech.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/layout/via.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/netlist/output.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/schematic/cellview.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/spirit/name.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/util/interval.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/util/io.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/util/sorted_map.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/util/sorted_vector.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/oa/oa_io.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp/tuple.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/util/io.cpp

  )

target_include_directories(test_main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(test_main
  PUBLIC
  cbag
  fmt::fmt
  yaml-cpp
  Catch2::Catch2
  spdlog
  )

install(TARGETS
  test_main
  RUNTIME DESTINATION ${CBAG_BINARY_DIR}
  LIBRARY DESTINATION ${CBAG_LIBRARY_DIR}
  )
