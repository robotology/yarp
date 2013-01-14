
# Copyright (C) 2010 Philipp Robbel
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.6.4)

IF (COMPILE_DEVICE_LIBRARY)
  PREPARE_DEVICE(primesensecamera TYPE ServerKinect INCLUDE "ServerKinect.h" WRAPPER primesensecamera)
  PREPARE_DEVICE(kinect TYPE KinectDeviceDriver INCLUDE "KinectDeviceDriver.h" WRAPPER primesensecamera)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (SKIP_primesensecamera AND SKIP_kinect)
  SET(SKIP_all_primesensecamera TRUE)
ENDIF (SKIP_primesensecamera AND SKIP_kinect)

IF (NOT SKIP_all_primesensecamera)
  FIND_PACKAGE(YARP REQUIRED)
  INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
  LINK_LIBRARIES(${YARP_LIBRARIES})

  # Include libfreenect library from openkinect repository at https://github.com/OpenKinect/openkinect
  FIND_PATH(LIBFREENECT_INCLUDE libfreenect.h)
  FIND_LIBRARY(LIBFREENECT_LIBRARIES freenect ${LIBFREENECT_INCLUDE}/../lib ${LIBFREENECT_INCLUDE}/../build/lib)
  SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${LIBFREENECT_INCLUDE}/../cmake_modules)

  FIND_PACKAGE(libusb-1.0 REQUIRED)

  # Tell cmake about the include directories
  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${LIBFREENECT_INCLUDE} ${LIBUSB_1_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIR} ${LIBUSB_1_INCLUDE_DIR}/libusb-1.0)
  LINK_LIBRARIES(${LIBFREENECT_LIBRARIES} ${USB_LIBRARIES})

  #ADD_DEFINITIONS(-DDEBUG -g)
  ADD_LIBRARY(kinect KinectDeviceDriver.cpp KinectDeviceDriver.h)

  IF (NOT COMPILE_DEVICE_LIBRARY)
    LINK_LIBRARIES(kinect)
    ADD_EXECUTABLE(example KinectExample.cpp)
  ENDIF (NOT COMPILE_DEVICE_LIBRARY)

ENDIF (NOT SKIP_all_primesensecamera)
