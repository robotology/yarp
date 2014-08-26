# Copyright (C) 2010 Philipp Robbel
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(primesensecamera TYPE ServerKinect INCLUDE "ServerKinect.h" WRAPPER primesensecamera)
  yarp_prepare_device(kinect TYPE KinectDeviceDriver INCLUDE "KinectDeviceDriver.h" WRAPPER primesensecamera)
endif()

if(SKIP_primesensecamera AND SKIP_kinect)
  set(SKIP_all_primesensecamera TRUE)
endif()

if(NOT SKIP_all_primesensecamera)
  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  link_libraries(${YARP_LIBRARIES})

  # Include libfreenect library from openkinect repository at https://github.com/OpenKinect/openkinect
  find_path(LIBFREENECT_INCLUDE libfreenect.h)
  find_library(LIBFREENECT_LIBRARIES freenect ${LIBFREENECT_INCLUDE}/../lib ${LIBFREENECT_INCLUDE}/../build/lib)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${LIBFREENECT_INCLUDE}/../cmake_modules)

  find_package(libusb-1.0 REQUIRED)

  # Tell cmake about the include directories
  include_directories(${CMAKE_SOURCE_DIR} ${LIBFREENECT_INCLUDE} ${LIBUSB_1_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIR} ${LIBUSB_1_INCLUDE_DIR}/libusb-1.0)
  link_libraries(${LIBFREENECT_LIBRARIES} ${USB_LIBRARIES})

  #add_definitions(-DDEBUG -g)
  yarp_add_plugin(kinect KinectDeviceDriver.cpp KinectDeviceDriver.h)

  if(NOT COMPILE_DEVICE_LIBRARY)
    link_libraries(kinect)
    add_executable(example KinectExample.cpp)
  endif()

endif()
