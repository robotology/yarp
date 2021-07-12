# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_executable(harness_rosmsg)

target_sources(harness_rosmsg PRIVATE ROSPropertiesTest.cpp)

target_link_libraries(harness_rosmsg PRIVATE YARP_harness
                                             YARP::YARP_os
                                             YARP::YARP_rosmsg)

set_property(TARGET harness_rosmsg PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_rosmsg)
