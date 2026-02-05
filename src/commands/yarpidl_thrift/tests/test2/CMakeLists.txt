# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

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


# Test yarp_idl_to_dir and yarp_add_idl
add_test(
  NAME idl::thrift::demo::run
  COMMAND ${CMAKE_CTEST_COMMAND} -VV --output-on-failure
                                 --build-and-test "${CMAKE_CURRENT_SOURCE_DIR}/demo"
                                                  "${CMAKE_CURRENT_BINARY_DIR}/demo"
                                 --build-two-config
                                 ${build_generator}
                                 --build-project demo
                                 --build-options ${build_options}
                                                 -DYCM_DIR=${YCM_DIR}
                                                 -DYARP_DIR=${CMAKE_BINARY_DIR}
                                                 -DALLOW_IDL_GENERATION=TRUE
                                                 -DCMAKE_EXE_LINKER_FLAGS_PROFILE='--coverage'
                                 --test-command ${CMAKE_CTEST_COMMAND} ${build_generator} --output-on-failure -V
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
set_tests_properties(idl::thrift::demo::run PROPERTIES FIXTURES_SETUP yarpidl_thrift_demo_run)
set_tests_properties(idl::thrift::demo::run PROPERTIES LABELS "yarp::idl::thrift")

# Cleanup
#add_test(
#  NAME idl::thrift::demo::cleanup
#  COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/demo"
#)
#set_tests_properties(idl::thrift::demo::cleanup PROPERTIES FIXTURES_CLEANUP yarpidl_thrift_demo_run)
#set_tests_properties(idl::thrift::demo::cleanup PROPERTIES LABELS "yarp::idl::thrift")

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_CURRENT_BINARY_DIR}/demo")
