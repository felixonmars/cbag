add_executable(test_main
  ${CMAKE_SOURCE_DIR}/test/main.cpp
  ${CMAKE_SOURCE_DIR}/test/cbag/util/sorted_vector.cpp
  )

target_link_libraries(test_main
  PUBLIC
  cbag
  )


# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(test_main PROPERTIES
  INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
  )
