add_executable(write_tech_info
  ${CMAKE_CURRENT_SOURCE_DIR}/write_tech_info.cpp
  )

# shared library dependencies
target_link_libraries(write_tech_info
  PUBLIC
  cbag
  )

add_executable(write_cv_yaml
  ${CMAKE_CURRENT_SOURCE_DIR}/write_cv_yaml.cpp
  )

# shared library dependencies
target_link_libraries(write_cv_yaml
  PUBLIC
  cbag
  )

add_executable(print_gds
  ${CMAKE_CURRENT_SOURCE_DIR}/print_gds.cpp
  )

# shared library dependencies
target_link_libraries(print_gds
  PUBLIC
  cbag
  )


install(TARGETS
  write_tech_info write_cv_yaml
  RUNTIME DESTINATION ${CBAG_BINARY_DIR}
  LIBRARY DESTINATION ${CBAG_LIBRARY_DIR}
  )
