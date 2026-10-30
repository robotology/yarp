# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT TARGET YARP::YARP_manager)
  return()
endif()

include(YarpCatchUtils)

add_executable(harness_manager)

target_sources(harness_manager
  PRIVATE
    ParamResolverTest.cpp
    TextParserTest.cpp
)

target_link_libraries(harness_manager
  PRIVATE
    YARP_harness
    YARP::YARP_manager
)

set_property(TARGET harness_manager PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_manager)
