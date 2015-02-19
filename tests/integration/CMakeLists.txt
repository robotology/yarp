# Copyright: (C) 2014 iCub Facility
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

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

# Generate a matrix of SWIG tests
foreach (lang PYTHON JAVA LUA RUBY CSHARP PERL TCL)
  foreach (swig 1.3.40 2.0.12 3.0.2)
    set(test_name "integration::swig::${lang}::${swig}")
    add_test(${test_name} ${CMAKE_SOURCE_DIR}/scripts/admin/check-bindings.sh ${swig} ${lang})
    set_property(TEST ${test_name} PROPERTY ENVIRONMENT "YARP_ROOT=${CMAKE_SOURCE_DIR}" "YARP_DIR=${CMAKE_BINARY_DIR}")
    set_property(TEST ${test_name} PROPERTY WORKING_DIRECTORY ${YARP_BINARY_DIR})
  endforeach()
endforeach()

# Add a "runtime" test for using plugins against yarp with static/shared linking
add_test("integration::runtime" ${CMAKE_SOURCE_DIR}/scripts/admin/check-runtime.sh)
set_property(TEST "integration::runtime" PROPERTY WORKING_DIRECTORY ${YARP_BINARY_DIR})

set(SIMPLE_TESTS 
  http # Sanity check for YARP http support against curl
  rpc  # Make sure replies get through when port closed immediately
)

foreach(test ${SIMPLE_TESTS})
  add_test("integration::${test}" ${CMAKE_CURRENT_SOURCE_DIR}/check-${test}.sh)
  set_property(TEST "integration::${test}" PROPERTY WORKING_DIRECTORY ${YARP_BINARY_DIR})
endforeach()
