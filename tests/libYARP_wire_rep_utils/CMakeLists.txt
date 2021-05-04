# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(NOT TARGET YARP::YARP_wire_rep_utils)
  return()
endif()

add_executable(harness_wire_rep_utils)

target_sources(harness_wire_rep_utils PRIVATE WireTest.cpp)

target_include_directories(harness_wire_rep_utils PRIVATE ${hmac_INCLUDE_DIRS})

target_link_libraries(harness_wire_rep_utils PRIVATE YARP_harness
                                                     YARP::YARP_os
                                                     YARP::YARP_wire_rep_utils)

set_property(TARGET harness_wire_rep_utils PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_wire_rep_utils)
