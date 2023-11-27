# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# This test check if the yarp executable is working
add_test(NAME misc::check_yarp_version COMMAND ${YARP_BINARY_DIR}/bin/yarp --version)
set_property(TEST misc::check_yarp_version PROPERTY ENVIRONMENT "YARP_DATA_DIRS=${YARP_BINARY_DIR}/share/yarp")

# This test check if all the plugins are working correctly (the test will fail if a single plugin cannot be opened)
add_test(NAME misc::check_yarp_plugin COMMAND ${YARP_BINARY_DIR}/bin/yarp plugin --all)
set_property(TEST misc::check_yarp_plugin PROPERTY ENVIRONMENT "YARP_DATA_DIRS=${YARP_BINARY_DIR}/share/yarp")

# Similar to the previous test, but using yarpdev. Only device plugins are checked.
add_test(NAME misc::check_yarpdev_list COMMAND ${YARP_BINARY_DIR}/bin/yarpdev --list)
set_property(TEST misc::check_yarpdev_list PROPERTY ENVIRONMENT "YARP_DATA_DIRS=${YARP_BINARY_DIR}/share/yarp")

if(NOT YARP_ENABLE_INTEGRATION_TESTS)
  return()
endif()

# Generate a matrix of SWIG tests
foreach(lang PYTHON JAVA LUA RUBY CSHARP TCL PERL)
  foreach(swig
    1.3.40
    2.0.12
    3.0.8
  )
    set(test_name "integration::swig::${lang}::${swig}")
    if("${test_name}" MATCHES "^integration::swig::PYTHON::1.3.37" OR
       "${test_name}" MATCHES "^integration::swig::(RUBY|LUA)::1.3.(37|38|39|40)$" OR
       "${test_name}" MATCHES "^integration::swig::(LUA)::2.0.(1|2|3|4|5|6)$")
      # Skip tests that are known to be broken due to swig
    else()
      add_test(${test_name} ${CMAKE_CURRENT_SOURCE_DIR}/check-bindings.sh ${swig} ${lang})
      set_property(TEST ${test_name} PROPERTY ENVIRONMENT "YARP_ROOT=${CMAKE_SOURCE_DIR}" "YARP_DIR=${CMAKE_BINARY_DIR}")
      set_property(TEST ${test_name} PROPERTY WORKING_DIRECTORY ${YARP_BINARY_DIR})
    endif()
  endforeach()
endforeach()

# Add a "runtime" test for using plugins against yarp with static/shared linking
add_test("integration::runtime" ${CMAKE_CURRENT_SOURCE_DIR}/check-runtime.sh)
set_property(TEST "integration::runtime" PROPERTY WORKING_DIRECTORY ${YARP_BINARY_DIR})
set_property(TEST "integration::runtime" PROPERTY TIMEOUT 2000)

set(SIMPLE_TESTS
  http  # Sanity check for YARP http support against curl
  rpc   # Make sure replies get through when port closed immediately
#  mcast # Test mcast protocol
)

foreach(test ${SIMPLE_TESTS})
  add_test("integration::${test}" ${CMAKE_CURRENT_SOURCE_DIR}/check-${test}.sh)
  set_property(TEST "integration::${test}" PROPERTY WORKING_DIRECTORY ${YARP_BINARY_DIR})
endforeach()
