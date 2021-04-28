# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

add_executable(harness_conf)

target_sources(harness_conf
  PRIVATE
    dirs.cpp
    environment.cpp
    numeric.cpp
    string.cpp)

target_link_libraries(harness_conf
  PRIVATE
    YARP_harness_no_network
    YARP::YARP_conf)

set_property(TARGET harness_conf PROPERTY FOLDER "Test")


yarp_catch_discover_tests(harness_conf)
