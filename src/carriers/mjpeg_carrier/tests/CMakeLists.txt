# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_executable(harness_carrier_mjpeg)
target_sources(harness_carrier_mjpeg
  PRIVATE
    mjpeg.cpp
)

target_link_libraries(harness_carrier_mjpeg
  PRIVATE
    YARP_harness
    YARP::YARP_os
    YARP::YARP_sig
)

set_property(TARGET harness_carrier_mjpeg PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_carrier_mjpeg)
