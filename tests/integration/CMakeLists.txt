# Copyright: (C) 2014 iCub Facility
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

option(YARP_TEST_INTEGRATION "Run integration tests (UNIX only)" FALSE)
mark_as_advanced(YARP_TEST_INTEGRATION)

if(YARP_TEST_INTEGRATION)

  add_test("integration::ros" ${CMAKE_SOURCE_DIR}/scripts/admin/check-ros.sh)
  set_property(TEST "integration::ros" PROPERTY ENVIRONMENT "YARP_DIR=${CMAKE_BINARY_DIR}")

  foreach (lang PYTHON JAVA LUA RUBY CSHARP PERL TCL)
    foreach (swig 1.3.40 2.0.12 3.0.2)
      add_test("integration::swig::${lang}::${swig}" ${CMAKE_SOURCE_DIR}/scripts/admin/check-bindings.sh ${swig} ${lang})
      set_property(TEST "integration::swig::${lang}::${swig}" PROPERTY ENVIRONMENT "YARP_ROOT=${CMAKE_SOURCE_DIR}")
    endforeach()
  endforeach()

  add_test("integration::runtime" ${CMAKE_SOURCE_DIR}/scripts/admin/check-runtime.sh)
  set_property(TEST "integration::runtime" PROPERTY WORKING_DIRECTORY ${YARP_BINARY_DIR})

endif()
