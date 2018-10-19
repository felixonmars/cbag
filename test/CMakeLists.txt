add_executable(test_main
  ${CMAKE_SOURCE_DIR}/test/main.cpp
  ${CMAKE_SOURCE_DIR}/test/cbag/util/interval.cpp
  ${CMAKE_SOURCE_DIR}/test/cbag/util/sorted_vector.cpp
  ${CMAKE_SOURCE_DIR}/test/cbag/util/vector_map.cpp
  )

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(test_main PROPERTIES
  INSTALL_RPATH "${CBAG_LIBRARY_DIR}:${Boost_LIBRARY_DIRS}"
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
