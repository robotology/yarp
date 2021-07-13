# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_executable(harness_dev)
target_sources(harness_dev
  PRIVATE
    AnalogWrapperTest.cpp
    ControlBoardRemapperTest.cpp
    ControlBoardWrapper2Test.cpp
    FrameTransformClientTest.cpp
    newFrameTransformClientTest.cpp
    GroupDriverTest.cpp
    MapGrid2DTest.cpp
    Navigation2DClientTest.cpp
    MultipleAnalogSensorsInterfacesTest.cpp
    PolyDriverTest.cpp
    robotDescriptionTest.cpp
    fakeFrameGrabberTest.cpp
    frameGrabberCropperTest.cpp
    frameGrabber_nws_yarpTest.cpp
    grabberDualTest.cpp
)

target_link_libraries(harness_dev
  PRIVATE
    YARP_harness
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_dev
)

if(TARGET YARP::YARP_math)
  target_link_libraries(harness_dev PRIVATE YARP::YARP_math)
else()
  set(_disabled_files
    FrameTransformClientTest.cpp
    newFrameTransformClientTest.cpp
    Map2DClientTest.cpp
    Navigation2DClientTest.cpp
    MapGrid2DTest.cpp
  )
  set_source_files_properties(${_disabled_files} PROPERTIES HEADER_FILE_ONLY ON)
endif()

set_property(TARGET harness_dev PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_dev)
