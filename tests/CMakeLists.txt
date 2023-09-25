# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause


if(NOT YARP_COMPILE_TESTS)
  return()
endif()

#########################################################################
# Prepare 'build_generator' and 'build_options' variables
# for tests in subdirectories
set(build_generator --build-generator "${CMAKE_GENERATOR}")
if(CMAKE_GENERATOR_PLATFORM)
  list(APPEND build_generator --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}")
endif()
if(CMAKE_GENERATOR_TOOLSET)
  list(APPEND build_generator --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}")
endif()
if(CMAKE_CONFIGURATION_TYPES)
  list(APPEND build_generator --build-config $<CONFIGURATION>)
endif()

set(build_options -Wno-dev)
if(CMAKE_BUILD_TYPE)
  list(APPEND build_options -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
endif()
if(YARP_TEST_LAUNCHER)
  set(_test_launcher "-DYARP_TEST_LAUNCHER=${YARP_TEST_LAUNCHER}")
  string(REPLACE ";" "\;" _test_launcher "${_test_launcher}")
  list(APPEND build_options "${_test_launcher}")
endif()
list(APPEND build_options -DYARP_TEST_TIMEOUT=${YARP_TEST_TIMEOUT})

# Add the CMAKE_BINARY_DIR as a macro so the testing infrastructure
# can find the device compiled as dynamic plugins
file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}" CMAKE_NATIVE_BINARY_DIR)
file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/conf" TEST_DATA_DIR)
if(CMAKE_HOST_WIN32)
  string(REPLACE "\\" "\\\\" CMAKE_NATIVE_BINARY_DIR "${CMAKE_NATIVE_BINARY_DIR}")
  string(REPLACE "\\" "\\\\" TEST_DATA_DIR "${TEST_DATA_DIR}")
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/YarpBuildLocation.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/YarpBuildLocation.h"
                @ONLY)

#########################################################################
# Main harness target

add_library(YARP_harness STATIC)
add_library(YARP::YARP_harness ALIAS YARP_harness)

target_sources(YARP_harness
  PRIVATE
    harness.cpp
    harness.h
)

target_link_libraries(YARP_harness
  PRIVATE
    YARP::YARP_conf
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_serversql
  PUBLIC
    YARP::YARP_catch2
)

target_include_directories(YARP_harness
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_property(TARGET YARP_harness PROPERTY FOLDER "Test")

#########################################################################
# Main harness target without network

add_library(YARP_harness_no_network STATIC)
add_library(YARP::YARP_harness_no_network ALIAS YARP_harness_no_network)

target_compile_definitions(YARP_harness_no_network PUBLIC WITHOUT_NETWORK)

target_sources(YARP_harness_no_network
  PRIVATE
    harness.cpp
    harness.h
)

target_link_libraries(YARP_harness_no_network
  PRIVATE
    YARP::YARP_conf
  PUBLIC
    YARP::YARP_catch2
)

target_include_directories(YARP_harness_no_network
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_property(TARGET YARP_harness_no_network PROPERTY FOLDER "Test")

#########################################################################
# Add subdirectories containing the actual tests

add_subdirectory(misc)

add_subdirectory(yarpidl_thrift)
add_subdirectory(yarpidl_rosmsg)

add_subdirectory(devices)

add_subdirectory(integration)
add_subdirectory(harness_tests)
