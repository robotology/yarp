# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# Test yarp_idl_to_dir and yarp_add_idl
add_test(
  NAME idl::rosmsg::demo::run
  COMMAND ${CMAKE_CTEST_COMMAND} --build-and-test "${CMAKE_CURRENT_SOURCE_DIR}/demo"
                                                  "${CMAKE_CURRENT_BINARY_DIR}/demo"
                                 --build-two-config
                                 ${build_generator}
                                 --build-project demo
                                 --build-options ${build_options}
                                                 -DYCM_DIR=${YCM_DIR}
                                                 -DYARP_DIR=${CMAKE_BINARY_DIR}
                                                 -DALLOW_IDL_GENERATION=TRUE
                                 --test-command ${CMAKE_CTEST_COMMAND} ${build_generator}
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
set_tests_properties(idl::rosmsg::demo::run PROPERTIES FIXTURES_SETUP yarpidl_rosmsg_demo_run)
set_tests_properties(idl::rosmsg::demo::run PROPERTIES LABELS "yarp::idl::rosmsg")

# Cleanup
add_test(
  NAME idl::rosmsg::demo::cleanup
  COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/demo"
)
set_tests_properties(idl::rosmsg::demo::cleanup PROPERTIES FIXTURES_CLEANUP yarpidl_thrift_demo_run)
set_tests_properties(idl::rosmsg::demo::cleanup PROPERTIES LABELS "yarp::idl::rosmsg")

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_CURRENT_BINARY_DIR}/demo")
