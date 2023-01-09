# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include(YarpCatchUtils)



add_executable(harness_dev_FrameTransformServer)

target_sources(harness_dev_FrameTransformServer
  PRIVATE
    FrameTransformServerTest.cpp
)

target_link_libraries(harness_dev_FrameTransformServer
  PRIVATE
    YARP_harness
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_dev
    YARP::YARP_dev_tests
)

if(TARGET YARP::YARP_math)
  target_link_libraries(harness_dev_FrameTransformServer
  PRIVATE
    YARP::YARP_math
  )
else()
  set(_disabled_files
    FrameTransformServerTest.cpp
  )
  set_source_files_properties(${_disabled_files} PROPERTIES HEADER_FILE_ONLY ON)
endif()

set_property(TARGET harness_dev_FrameTransformServer PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_dev_FrameTransformServer)
