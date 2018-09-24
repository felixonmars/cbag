add_executable(write_tech_info
  ${CMAKE_SOURCE_DIR}/util/write_tech_info.cpp
  )

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(write_tech_info PROPERTIES
  INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
  )

# shared library dependencies
target_link_libraries(write_tech_info
  cbag
  cbagoa
  )

install(TARGETS
  write_tech_info
  RUNTIME DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin
  LIBRARY DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/lib
  )
