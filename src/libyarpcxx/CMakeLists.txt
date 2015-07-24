# Copyright: (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(CREATE_YARPC)

  option(CREATE_YARPCXX "Compile experimental stable C++ library" FALSE)

  if(CREATE_YARPCXX)

    project(yarpcxx)

    # TODO install headers

    if(YARP_COMPILE_TESTS)

      # pick up yarp.h
      include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libyarpc)

      # pick up experimental C++ headers
      include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

      add_executable(yarpcxx_test1 yarpcxx_test1.cpp)
      target_link_libraries(yarpcxx_test1 yarpc)

      add_executable(yarpcxx_test2 yarpcxx_test2.cpp)
      target_link_libraries(yarpcxx_test2 yarpc)

      set(EXAMPLE_DIR ${CMAKE_SOURCE_DIR}/example)
      add_executable(yarpcxx_test_threads ${EXAMPLE_DIR}/os/threads.cpp)
      target_link_libraries(yarpcxx_test_threads yarpc)

      add_executable(yarpcxx_test_simple_sender ${EXAMPLE_DIR}/os/simple_sender.cpp)
      target_link_libraries(yarpcxx_test_simple_sender yarpc)

      add_executable(yarpcxx_test_simple_receiver ${EXAMPLE_DIR}/os/simple_receiver.cpp)
      target_link_libraries(yarpcxx_test_simple_receiver yarpc)

    endif()

  endif()

endif()
