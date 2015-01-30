# Copyright: (C) 2014 iCub Facility
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

option(YARP_TEST_INTEGRATION "Run integration tests (UNIX only)" FALSE)
mark_as_advanced(YARP_TEST_INTEGRATION)

if(YARP_TEST_INTEGRATION)
  set(test_family "integration::ros")
  set(test_driver ${CMAKE_CURRENT_SOURCE_DIR}/ros/index.sh)
  execute_process(COMMAND ${test_driver} OUTPUT_VARIABLE test_list)
  separate_arguments(test_list)
  message(STATUS ${test_list})
  foreach(test ${test_list})
    set(test_name "${test_family}::${test}")
    message(STATUS "test ${test_name}")
    add_test(${test_name} ${test_driver} ${test})
    set_property(TEST ${test_name} PROPERTY ENVIRONMENT "YARP_DIR=${CMAKE_BINARY_DIR}")
  endforeach()

  foreach (lang PYTHON JAVA LUA RUBY CSHARP PERL TCL)
    foreach (swig 1.3.40 2.0.12 3.0.2)
      add_test("integration::swig::${lang}::${swig}" ${CMAKE_SOURCE_DIR}/scripts/admin/check-bindings.sh ${swig} ${lang})
      set_property(TEST "integration::swig::${lang}::${swig}" PROPERTY ENVIRONMENT "YARP_ROOT=${CMAKE_SOURCE_DIR}" "YARP_DIR=${CMAKE_BINARY_DIR}")
    endforeach()
  endforeach()

  add_test("integration::runtime" ${CMAKE_SOURCE_DIR}/scripts/admin/check-runtime.sh)
  set_property(TEST "integration::runtime" PROPERTY WORKING_DIRECTORY ${YARP_BINARY_DIR})

  add_test("integration::http" ${CMAKE_CURRENT_SOURCE_DIR}/check-http.sh)
  set_property(TEST "integration::http" PROPERTY WORKING_DIRECTORY ${YARP_BINARY_DIR})

endif()
