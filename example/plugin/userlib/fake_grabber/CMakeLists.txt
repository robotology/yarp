# Copyright: (C) 2010 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

# Declare a device called "fake_grabber", with C++ type
# "FakeFrameGrabber" defined in FakeFrameGrabber.h.  This code sets
# up a CMake option for selecting the device, and prepares
# some initialization boilerplate code if needed.
# If the device is not selected, "SKIP_foo" will be defined.
yarp_prepare_plugin(fake_grabber
                    CATEGORY device
                    TYPE FakeFrameGrabber
                    INCLUDE FakeFrameGrabber.h
                    EXTRA_CONFIG WRAPPER=grabber)

if(NOT SKIP_fake_grabber)
  # Find any libraries needed - just YARP for this example
  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  # ... find and use other needed libraries ...
  yarp_add_plugin(fake_grabber FakeFrameGrabber.cpp FakeFrameGrabber.h)
  target_link_libraries(fake_grabber ${YARP_LIBRARIES})

  option(COMPILE_TEST "Compile fake grabber test")
  if(COMPILE_TEST)
    add_executable(test_fake_grabber test_fake_grabber.cpp)
    target_link_libraries(test_fake_grabber fake_grabber)
  endif()
endif()
