# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT TARGET YARP::YARP_manager)
  return()
endif()

include(YarpCatchUtils)

add_executable(harness_manager)

target_sources(harness_manager
  PRIVATE
    AppParamsTest.cpp
    ParamResolverTest.cpp
    TextParserTest.cpp
)

target_link_libraries(harness_manager
  PRIVATE
    YARP_harness
    YARP::YARP_manager
)

target_compile_definitions(harness_manager
  PRIVATE
    MANAGER_TEST_APPS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/apps"
    MANAGER_TEST_INVALID_APPS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/apps_invalid"
)

set_property(TARGET harness_manager PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_manager)
