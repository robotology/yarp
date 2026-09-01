# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include(YarpCatchUtils)

add_executable(harness_profiler)

target_sources(harness_profiler PRIVATE
  NetworkProfilerBasicTest.cpp
)

target_include_directories(harness_profiler PRIVATE ${hmac_INCLUDE_DIRS})

target_link_libraries(harness_profiler
  PRIVATE
    YARP_harness
    YARP::YARP_os
    YARP::YARP_profiler
    YARP::YARP_serversql
)

set_property(TARGET harness_profiler PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_profiler)
