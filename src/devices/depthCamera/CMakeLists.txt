# Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
# Authors: Alberto Cardellino, Andrea Ruzzenenti
# email:   andrea.ruzzenenti@iit.it
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(WIN32)
find_path(OpenNI2_INCLUDE_LOCAL NAMES OpenNI.h HINTS $ENV{OpenNI2_DIR}/Include)
find_library(OpenNI2_LIBRARY NAMES OpenNI2 libOpenNI2 HINTS $ENV{OpenNI2_DIR}/Lib $ENV{OpenNI2_DIR}/Redist)
else(WIN32)
find_path(OpenNI2_INCLUDE_LOCAL NAMES OpenNI.h HINTS $ENV{OpenNI2_DIR}/include)
find_library(OpenNI2_LIBRARY NAMES OpenNI2 libOpenNI2 HINTS $ENV{OpenNI2_DIR}/lib $ENV{OpenNI2_DIR}/redist)
endif(WIN32)
if(OpenNI2_INCLUDE_LOCAL AND OpenNI2_LIBRARY)
  set(OpenNI2_FOUND TRUE)
else()
  set(OpenNI2_FOUND FALSE)
endif()


yarp_prepare_plugin(depthCamera
                    CATEGORY             device
                    TYPE                 yarp::dev::depthCameraDriver
                    INCLUDE              depthCameraDriver.h
                    EXTRA_CONFIG WRAPPER=RGBDSensorWrapper
                    DEPENDS              "CREATE_DEVICE_LIBRARY_MODULES;OpenNI2_FOUND")

if(ENABLE_depthCamera)

  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)

  include_directories(SYSTEM ${OpenNI2_INCLUDE_LOCAL})
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})

  yarp_add_plugin(yarp_depthCamera depthCameraDriver.cpp depthCameraDriver.h depthCameraImpl.hpp)
  
  target_link_libraries(yarp_depthCamera YARP_OS YARP_sig YARP_dev ${OpenNI2_LIBRARY})

  yarp_install(TARGETS     yarp_depthCamera
               EXPORT      YARP
               COMPONENT   runtime
               LIBRARY     DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE     DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

  yarp_install(FILES       depthCamera.ini
               COMPONENT   runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()
