# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

add_executable(harness_dev AnalogWrapperTest.cpp
                           ControlBoardRemapperTest.cpp
                           ControlBoardWrapper2Test.cpp
                           FrameTransformClientTest.cpp
                           GroupDriverTest.cpp
                           MapGrid2DTest.cpp
                           Navigation2DClientTest.cpp
                           MultipleAnalogSensorsInterfacesTest.cpp
                           PolyDriverTest.cpp
                           robotDescriptionTest.cpp
                           TestFrameGrabberTest.cpp)

target_link_libraries(harness_dev PRIVATE YARP_harness
                                          YARP::YARP_OS
                                          YARP::YARP_sig
                                          YARP::YARP_dev)

if(TARGET YARP::YARP_math)
  target_link_libraries(harness_dev PRIVATE YARP::YARP_math)
else()
  set(_disabled_files FrameTransformClientTest.cpp
                      Navigation2DClientTest.cpp
                      MapGrid2DTest.cpp)

  if(NOT ${CMAKE_MINIMUM_REQUIRED_VERSION} VERSION_LESS 3.9)
    message(AUTHOR_WARNING "DISABLED property is supported since CMake 3.9, you can remove this check")
  endif()
 if(NOT ${CMAKE_VERSION} VERSION_LESS 3.9)
   set_source_files_properties(${_disabled_files} PROPERTIES HEADER_FILE_ONLY ON)
 else()
    get_property(_sources TARGET harness_dev PROPERTY SOURCES)
    foreach(_file IN LISTS _disabled_files)
      list(REMOVE_ITEM _sources ${_file})
    endforeach()
    set_property(TARGET harness_dev PROPERTY SOURCES ${_sources})
 endif()
endif()

set_property(TARGET harness_dev PROPERTY FOLDER "Test")

yarp_parse_and_add_catch_tests(harness_dev)
