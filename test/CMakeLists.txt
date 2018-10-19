add_executable(test_main
  ${CMAKE_SOURCE_DIR}/test/main.cpp
  ${CMAKE_SOURCE_DIR}/test/cbag/util/interval.cpp
  ${CMAKE_SOURCE_DIR}/test/cbag/util/sorted_vector.cpp
  ${CMAKE_SOURCE_DIR}/test/cbag/util/vector_map.cpp
  )

target_link_libraries(test_main
  PUBLIC
  cbag
  )

install(TARGETS
  test_main
  RUNTIME DESTINATION ${CBAG_BINARY_DIR}
  LIBRARY DESTINATION ${CBAG_LIBRARY_DIR}
  )
