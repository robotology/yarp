# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.


if(NOT YARP_COMPILE_TESTS)
  return()
endif()
# set(YARP_TEST_LAUNCHER ${VALGRIND_MEMCHECK_COMMAND})

#########################################################################
# Options

include(CMakeDependentOption)

if(YARP_ENABLE_BROKEN_TESTS)
  add_definitions(-DENABLE_BROKEN_TESTS)
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
# Prepare 'build_generator', 'build_options' and 'build_module_path' variables
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

# Use only if cmake modules not installed by YARP are required by the test
set(build_module_path "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")
string(REPLACE ";" "\;" build_module_path "${build_module_path}")

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
# Wrapper for the ParseAndAddCatchTests that also sets the
# SKIP_RETURN_CODE test property

set(PARSE_CATCH_TESTS_VERBOSE OFF CACHE INTERNAL "")
set(PARSE_CATCH_TESTS_NO_HIDDEN_TESTS OFF CACHE INTERNAL "")
set(PARSE_CATCH_TESTS_HIDDEN_TESTS_DISABLED ON CACHE INTERNAL "")
set(PARSE_CATCH_TESTS_ADD_TARGET_IN_TEST_NAME OFF CACHE INTERNAL "")
set(PARSE_CATCH_TESTS_ADD_FIXTURE_IN_TEST_NAME ON CACHE INTERNAL "")
set(PARSE_CATCH_TESTS_ADD_TO_CONFIGURE_DEPENDS ON CACHE INTERNAL "")
set(AdditionalCatchParameters "-s" "--use-colour" "yes")
set(OptionalCatchTestLauncher ${YARP_TEST_LAUNCHER})

include(ParseAndAddCatchTests)

function(yarp_parse_and_add_catch_tests _target)
  ParseAndAddCatchTests(${_target})
  get_property(_tests TARGET ${_target} PROPERTY ParseAndAddCatchTests_TESTS)
  foreach(_test IN LISTS _tests)
    set_property(TEST ${_test} PROPERTY TIMEOUT 120)
    set_property(TEST ${_test} PROPERTY SKIP_RETURN_CODE 255)
  endforeach()
  get_property(_sources TARGET ${_target} PROPERTY SOURCES)
  foreach(_source IN LISTS _sources)
    get_property(_header_only_set SOURCE ${_source} PROPERTY HEADER_FILE_ONLY SET)
    if(_header_only_set)
      get_property(_header_only SOURCE ${_source} PROPERTY HEADER_FILE_ONLY)
      if(_header_only)
        get_property(_tests SOURCE ${_source} PROPERTY ParseAndAddCatchTests_TESTS)
        foreach(_test IN LISTS _tests)
          set_property(TEST ${_test} PROPERTY DISABLED ON)
        endforeach()
      endif()
    endif()
  endforeach()
endfunction()


#########################################################################
# Main harness target

add_library(YARP_harness STATIC)

target_sources(YARP_harness PRIVATE harness.cpp
                                    harness.h)

target_link_libraries(YARP_harness PRIVATE YARP::YARP_os
                                           YARP::YARP_init
                                           YARP::YARP_serversql)

target_include_directories(YARP_harness PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${CMAKE_SOURCE_DIR}/extern/catch2)

set_property(TARGET YARP_harness PROPERTY FOLDER "Test")

#########################################################################
# Add subdirectories containing the actual tests

add_subdirectory(misc)

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
