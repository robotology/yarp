# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)
find_package(YARP COMPONENTS os dev REQUIRED)

# Declare a device called "fake_grabber", with C++ type
# "FakeFrameGrabber" defined in FakeFrameGrabber.h.  This code sets
# up a CMake option for selecting the device, and prepares
# some initialization boilerplate code if needed.
# If the device is not selected, "SKIP_foo" will be defined.
yarp_prepare_plugin(fake_grabber
                    CATEGORY device
                    TYPE FakeFrameGrabber
                    INCLUDE FakeFrameGrabber.h
                    EXTRA_CONFIG WRAPPER=grabberDual)

if(NOT SKIP_fake_grabber)
  yarp_add_plugin(fake_grabber)

  target_sources(fake_grabber PRIVATE FakeFrameGrabber.cpp
                                      FakeFrameGrabber.h)
  target_link_libraries(fake_grabber PRIVATE ${YARP_LIBRARIES})

  option(COMPILE_TEST "Compile fake grabber test")
  if(COMPILE_TEST)
    add_executable(test_fake_grabber)
    target_sources(test_fake_grabber PRIVATE test_fake_grabber.cpp
                                             FakeFrameGrabber.cpp
                                             FakeFrameGrabber.h)
    target_link_libraries(test_fake_grabber PRIVATE ${YARP_LIBRARIES})
  endif()
endif()
