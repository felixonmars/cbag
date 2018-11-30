

add_executable(test_main
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/netlist/output.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/schematic/cellview.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/spirit/name.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/util/interval.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/util/sorted_map.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/util/sorted_vector.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbagoa/oa_io.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/util/io.cpp

  )

target_include_directories(test_main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(test_main
  PRIVATE
  cbag
  fmt
  )

install(TARGETS
  test_main
  RUNTIME DESTINATION ${CBAG_BINARY_DIR}
  LIBRARY DESTINATION ${CBAG_LIBRARY_DIR}
  )
