# Copyright (C) 2010 Philipp Robbel
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

yarp_prepare_plugin(primesensecamera
                    CATEGORY device
                    TYPE yarp::dev::ServerKinect
                    INCLUDE "ServerKinect.h"
                    EXTRA_CONFIG WRAPPER=primesensecamera
                    DEPENDS "YARP_HAS_LIBUSB1")
yarp_prepare_plugin(kinect
                    CATEGORY device
                    TYPE yarp::dev::KinectDeviceDriver
                    INCLUDE "KinectDeviceDriver.h"
                    EXTRA_CONFIG WRAPPER=primesensecamera
                    DEPENDS "YARP_HAS_LIBUSB1")

if(NOT SKIP_primesensecamera OR NOT SKIP_kinect)
  # Include libfreenect library from openkinect repository at https://github.com/OpenKinect/openkinect
  find_path(LIBFREENECT_INCLUDE libfreenect.h)
  find_library(LIBFREENECT_LIBRARIES freenect ${LIBFREENECT_INCLUDE}/../lib ${LIBFREENECT_INCLUDE}/../build/lib)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${LIBFREENECT_INCLUDE}/../cmake_modules)

  # Tell cmake about the include directories
  include_directories(${CMAKE_SOURCE_DIR} ${LIBFREENECT_INCLUDE} ${Libusb1_INCLUDE_DIRS})

  #add_definitions(-DDEBUG -g)
  yarp_add_plugin(yarp_kinect KinectDeviceDriver.cpp
                              KinectDeviceDriver.h)

  target_link_libraries(yarp_kinect YARP::YARP_OS
                                    YARP::YARP_sig
                                    YARP::YARP_dev
                                    ${LIBFREENECT_LIBRARIES}
                                    ${USB_LIBRARIES})

  yarp_install(TARGETS yarp_kinect
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

  set_property(TARGET yarp_kinect PROPERTY FOLDER "Unsupported")

  if(YARP_COMPILE_TESTS)
    add_executable(test_kinect KinectExample.cpp
                               KinectDeviceDriver.cpp
                               KinectDeviceDriver.h)
    target_link_libraries(test_kinect YARP::YARP_OS
                                      YARP::YARP_init
                                      YARP::YARP_sig
                                      YARP::YARP_dev
                                      ${LIBFREENECT_LIBRARIES}
                                      ${USB_LIBRARIES})
  endif()

endif()
