# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_executable(harness_carriers)
target_sources(harness_carriers
  PRIVATE
    mjpeg.cpp
)

target_link_libraries(harness_carriers
  PRIVATE
    YARP_harness
    YARP::YARP_os
    YARP::YARP_sig
)

set_property(TARGET harness_carriers PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_carriers)
