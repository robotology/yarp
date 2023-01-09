# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include(YarpCatchUtils)



add_executable(harness_dev_fakeMotionControl)

target_sources(harness_dev_fakeMotionControl
  PRIVATE
    FakeMotionControlTest.cpp
)

target_link_libraries(harness_dev_fakeMotionControl
  PRIVATE
    YARP_harness
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_dev
    YARP::YARP_dev_tests
)

set_property(TARGET harness_dev_fakeMotionControl PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_dev_fakeMotionControl)
