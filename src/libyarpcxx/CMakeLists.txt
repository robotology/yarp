# Copyright: (C) 2010 Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

option(CREATE_YARPCXX "Compile experimental stable C++ library" FALSE)
mark_as_advanced(CREATE_YARPCXX)

if (CREATE_YARPCXX)

  project(yarpcxx)

  # pick up yarp.h
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libyarpc)

  # pick up experimental C++ headers
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

  add_executable(yarpcxx_test1 yarpcxx_test1.cpp)
  target_link_libraries(yarpcxx_test1 yarpc)

  add_executable(yarpcxx_test2 yarpcxx_test2.cpp)
  target_link_libraries(yarpcxx_test2 yarpc)

endif ()
