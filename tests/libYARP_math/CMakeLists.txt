# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT TARGET YARP::YARP_math)
  return()
endif()

add_executable(harness_math)

target_sources(harness_math
  PRIVATE
    MathTest.cpp
    Vec2DTest.cpp
    svdTest.cpp
    RandTest.cpp
)

target_link_libraries(harness_math
  PRIVATE
    YARP_harness
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_math
)

set_property(TARGET harness_math PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_math)
