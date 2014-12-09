# Copyright: (C) 2014 iCub Facility
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

option(YARP_TEST_INTEGRATION "Run integration tests (UNIX only)" FALSE)
mark_as_advanced(YARP_TEST_INTEGRATION)

if(YARP_TEST_INTEGRATION)

  add_test("integration::ros" ${CMAKE_SOURCE_DIR}/scripts/admin/check-ros.sh)
  set_property(TEST "integration::ros" PROPERTY ENVIRONMENT "YARP_DIR=${CMAKE_BINARY_DIR}")

endif()
