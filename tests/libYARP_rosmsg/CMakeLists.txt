# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

add_executable(harness_rosmsg)

target_sources(harness_rosmsg PRIVATE ROSPropertiesTest.cpp)

target_link_libraries(harness_rosmsg PRIVATE YARP_harness
                                             YARP::YARP_os
                                             YARP::YARP_rosmsg)

set_property(TARGET harness_rosmsg PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_rosmsg)
