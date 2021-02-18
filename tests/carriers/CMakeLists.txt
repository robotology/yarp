# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

add_executable(harness_carriers)
target_sources(harness_carriers PRIVATE mjpeg.cpp)

target_link_libraries(harness_carriers PRIVATE YARP_harness
                                               YARP::YARP_os
                                               YARP::YARP_sig)

set_property(TARGET harness_carriers PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_carriers)
