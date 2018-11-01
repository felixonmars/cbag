add_executable(test_main
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/util/interval.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/util/sorted_map.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbag/util/sorted_vector.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cbagyaml/cbagyaml.cpp
  )

target_link_libraries(test_main
  PUBLIC
  cbag
  cbagyaml
  PRIVATE
  fmt-header-only
  )

install(TARGETS
  test_main
  RUNTIME DESTINATION ${CBAG_BINARY_DIR}
  LIBRARY DESTINATION ${CBAG_LIBRARY_DIR}
  )
