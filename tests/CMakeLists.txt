# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause


if(NOT YARP_COMPILE_TESTS)
  return()
endif()

#########################################################################
# Options

include(CMakeDependentOption)

if(YARP_ENABLE_BROKEN_TESTS)
  add_definitions(-DENABLE_BROKEN_TESTS)
endif()

if(YARP_VALGRIND_TESTS)
  add_definitions(-DWITH_VALGRIND)
endif()

if(YARP_DISABLE_FAILING_TESTS)
  add_definitions(-DDISABLE_FAILING_TESTS)
endif()


#########################################################################
# Control where test libraries and executables are placed during the build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/tests")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/tests")


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
include_directories(${CMAKE_CURRENT_BINARY_DIR})


#########################################################################
# Wrapper for the catch_discover_tests that also enables colors, and sets
# the TIMEOUT and SKIP_RETURN_CODE test properties.

include(Catch)
function(yarp_catch_discover_tests _target)
  # Workaround to force catch_discover_tests to run tests under valgrind
  set_property(TARGET ${_target} PROPERTY CROSSCOMPILING_EMULATOR "${YARP_TEST_LAUNCHER}")
  catch_discover_tests(
    ${_target}
    EXTRA_ARGS "-s" "--use-colour" "yes"
    PROPERTIES
      TIMEOUT ${YARP_TEST_TIMEOUT}
      SKIP_RETURN_CODE 254
    )
endfunction()


#########################################################################
# Main harness target

add_library(YARP_harness STATIC)

target_sources(YARP_harness PRIVATE harness.cpp
                                    harness.h)

target_link_libraries(YARP_harness PRIVATE YARP::YARP_conf
                                           YARP::YARP_os
                                           YARP::YARP_init
                                           YARP::YARP_serversql)

target_include_directories(YARP_harness PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${CMAKE_SOURCE_DIR}/extern/catch2)

set_property(TARGET YARP_harness PROPERTY FOLDER "Test")

#########################################################################
# Main harness target without network

add_library(YARP_harness_no_network STATIC)

target_compile_definitions(YARP_harness_no_network PUBLIC WITHOUT_NETWORK)

target_sources(YARP_harness_no_network PRIVATE harness.cpp
                                               harness.h)

target_link_libraries(YARP_harness_no_network PRIVATE YARP::YARP_conf)

target_include_directories(YARP_harness_no_network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                                          ${CMAKE_SOURCE_DIR}/extern/catch2)

set_property(TARGET YARP_harness_no_network PROPERTY FOLDER "Test")

#########################################################################
# Add subdirectories containing the actual tests

add_subdirectory(misc)

add_subdirectory(libYARP_conf)
add_subdirectory(libYARP_os)
add_subdirectory(libYARP_os/impl)
add_subdirectory(libYARP_sig)
add_subdirectory(libYARP_rosmsg)
add_subdirectory(libYARP_dev)
add_subdirectory(libYARP_serversql)
add_subdirectory(libYARP_run)
add_subdirectory(libYARP_math)
add_subdirectory(libYARP_wire_rep_utils)
add_subdirectory(libYARP_robotinterface)

add_subdirectory(yarpidl_thrift)
add_subdirectory(yarpidl_rosmsg)

add_subdirectory(carriers)
add_subdirectory(devices)

add_subdirectory(integration)
