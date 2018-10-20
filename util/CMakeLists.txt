add_executable(write_tech_info
  ${CMAKE_CURRENT_SOURCE_DIR}/write_tech_info.cpp
  )

# shared library dependencies
target_link_libraries(write_tech_info
  PUBLIC
  cbag
  cbagoa
  PRIVATE
  fmt-header-only
  )

install(TARGETS
  write_tech_info
  RUNTIME DESTINATION ${CBAG_BINARY_DIR}
  LIBRARY DESTINATION ${CBAG_LIBRARY_DIR}
  )
