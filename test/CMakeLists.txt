#[[
add_executable(test_oa
  ${CMAKE_SOURCE_DIR}/test/test_oa.cpp
  )

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(test_oa PROPERTIES
  INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
  )

# shared library dependencies
target_link_libraries(test_oa
  cbagoa
  oaDM
  oaTech
  oaDesign
  dl
  )
]]

#[[
add_executable(test_spirit
  ${CMAKE_SOURCE_DIR}/test/test_spirit.cpp
  )

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(test_spirit PROPERTIES
  INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
  )

target_link_libraries(test_spirit
  cbag
  yaml-cpp
  boost_system
  )
]]

#[[
add_executable(read_prop
  ${CMAKE_SOURCE_DIR}/test/read_prop.cpp
  )

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(read_prop PROPERTIES
  INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
  )

# shared library dependencies
target_link_libraries(read_prop
  oaCommon
  oaBase
  oaPlugIn
  oaDM
  oaDMFileSysBase
  oaTech
  oaDesign
  ${CMAKE_DL_LIBS}
  )
]]

#[[
add_executable(test_yaml
  ${CMAKE_SOURCE_DIR}/test/test_yaml.cpp
  )

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(test_yaml PROPERTIES
  INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
  )

# shared library dependencies
target_link_libraries(test_yaml
  fmt-header-only
  yaml-cpp
  )
]]

#[[
add_executable(netmap_sample
  ${CMAKE_SOURCE_DIR}/test/netmap_sample.cpp
  )

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(netmap_sample PROPERTIES
  INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
  )

# shared library dependencies
target_link_libraries(netmap_sample
  cbag
  cbagyaml
  yaml-cpp
  boost_system
  )
]]

#[[
add_executable(test_polygon
  ${CMAKE_SOURCE_DIR}/test/test_polygon.cpp
  )

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(test_polygon PROPERTIES
  INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
  )

target_link_libraries(test_polygon
  cbag
  )
]]

#[[
add_executable(test_misc
  ${CMAKE_SOURCE_DIR}/test/test_misc.cpp
  )

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(test_misc PROPERTIES
  INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
  )
]]

add_executable(test_rtree
  ${CMAKE_SOURCE_DIR}/test/test_rtree.cpp
  )

# shared library dependencies
target_link_libraries(test_rtree
  cbag
  )

# append build directory to RPATH
# we do it this way now because we're running directly from the build directory.
set_target_properties(test_rtree PROPERTIES
  INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/lib:${Boost_LIBRARY_DIRS}"
  )
