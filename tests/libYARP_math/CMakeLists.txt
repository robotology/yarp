# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(NOT TARGET YARP::YARP_math)
  return()
endif()

add_executable(harness_math)

target_sources(harness_math PRIVATE MathTest.cpp
                                    Vec2DTest.cpp
                                    svdTest.cpp
                                    RandTest.cpp)

target_link_libraries(harness_math PRIVATE YARP_harness
                                           YARP::YARP_os
                                           YARP::YARP_sig
                                           YARP::YARP_math)

set_property(TARGET harness_math PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_math)
