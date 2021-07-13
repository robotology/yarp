# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_ENABLE_INTEGRATION_TESTS)
  return()
endif()

find_program(ROS_ROSCORE_EXECUTABLE NAMES roscore)
mark_as_advanced(ROS_ROSCORE_EXECUTABLE)
if(ROS_ROSCORE_EXECUTABLE)
  # Pull in all ROS tests, and add each as an individual ctest test
  set(test_family "integration::ros")
  set(test_driver ${CMAKE_CURRENT_SOURCE_DIR}/ros/index.sh)
  execute_process(COMMAND ${test_driver} OUTPUT_VARIABLE test_list OUTPUT_STRIP_TRAILING_WHITESPACE WORKING_DIRECTORY ${YARP_BINARY_DIR})
  separate_arguments(test_list)
  foreach(test ${test_list})
    set(test_name "${test_family}::${test}")
    add_test(${test_name} ${test_driver} ${test})
    set_property(TEST ${test_name} PROPERTY ENVIRONMENT "YARP_DIR=${CMAKE_BINARY_DIR}")
    set_property(TEST ${test_name} PROPERTY WORKING_DIRECTORY ${YARP_BINARY_DIR})
  endforeach()
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
