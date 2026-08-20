# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include(YarpCatchUtils)

add_executable(harness_stats)

target_sources(harness_stats
  PRIVATE
    StatsTest.cpp
)

target_link_libraries(harness_stats
  PRIVATE
    YARP_harness
    YARP::YARP_os
    YARP::YARP_stats
)

set_property(TARGET harness_stats PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_stats)
