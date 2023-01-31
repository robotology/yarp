# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_executable(harness_libyarpcv)
target_sources(harness_libyarpcv
  PRIVATE
    LibYarpCVTest.cpp
)

target_link_libraries(harness_libyarpcv
  PRIVATE
    YARP_harness
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_dev
    YARP::YARP_cv
)

set_property(TARGET harness_libyarpcv PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_libyarpcv)
