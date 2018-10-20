add_executable(test_main
  ${CMAKE_SOURCE_DIR}/test/main.cpp
  ${CMAKE_SOURCE_DIR}/test/cbag/util/interval.cpp
  ${CMAKE_SOURCE_DIR}/test/cbag/util/sorted_vector.cpp
  )

target_link_libraries(test_main
  PUBLIC
  cbag
  PRIVATE
  fmt-header-only
  )

install(TARGETS
  test_main
  RUNTIME DESTINATION ${CBAG_BINARY_DIR}
  LIBRARY DESTINATION ${CBAG_LIBRARY_DIR}
  )
