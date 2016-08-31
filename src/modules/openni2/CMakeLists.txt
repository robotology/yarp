# Copyright (C) 2013
# Author: Konstantinos Theofilis
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_DEVICE_LIBRARY)
  yarp_prepare_plugin(OpenNI2DeviceServer
                      CATEGORY device
                      TYPE yarp::dev::OpenNI2DeviceDriverServer
                      INCLUDE OpenNI2DeviceDriverServer.h)
  yarp_prepare_plugin(OpenNI2DeviceClient
                      CATEGORY device
                      TYPE yarp::dev::OpenNI2DeviceDriverClient
                      INCLUDE OpenNI2DeviceDriverClient.h)
  yarp_install(FILES openni2.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()

if (NOT SKIP_OpenNI2DeviceServer OR NOT SKIP_OpenNI2DeviceClient)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})

  find_path(OPENNI2_INCLUDE_LOCAL NAMES OpenNI.h HINTS $ENV{OPENNI2_INCLUDE} /usr/include/openni2)
  find_library(OPENNI2_LIBRARY NAMES OpenNI2 libOpenNI2.dylib libOpenNI2 HINTS $ENV{OPENNI2_REDIST})
  find_path(NITE2_INCLUDE_LOCAL NAMES NiTE.h HINTS $ENV{NITE2_INCLUDE})
  find_library(NITE2_LIBRARY NAMES NiTE2 HINTS $ENV{NITE2_REDIST64})
  if(NITE2_INCLUDE_LOCAL)
    set(NITE2_FOUND TRUE)
  else()
    set(NITE2_FOUND FALSE)
  endif()

  if(NOT NITE2_FOUND)
    message(STATUS "OpenNI2Device: NITE2 library was not found, skeleton extraction feature will be skipped by driver. "
                   "If you need this feature please install NITE2 library.")
  else()
    add_definitions(-DOPENNI2_DRIVER_USES_NITE2)
  endif()

  include_directories(SYSTEM ${OPENNI2_INCLUDE_LOCAL})
  if(NITE2_FOUND)
    include_directories(SYSTEM ${NITE2_INCLUDE_LOCAL})
  endif()

  yarp_add_plugin(yarp_OpenNI2Device OpenNI2DeviceDriverClient.cpp
                                     OpenNI2DeviceDriverClient.h
                                     OpenNI2DeviceDriverServer.cpp
                                     OpenNI2DeviceDriverServer.h
                                     OpenNI2SkeletonData.cpp
                                     OpenNI2SkeletonData.h
                                     OpenNI2SkeletonTracker.cpp
                                     OpenNI2SkeletonTracker.h)
  target_link_libraries(yarp_OpenNI2Device YARP_OS
                                           YARP_sig
                                           YARP_dev
                                           ${OPENNI2_LIBRARY})

  if(NITE2_FOUND)
    target_link_libraries(yarp_OpenNI2Device ${NITE2_LIBRARY})
  endif()

  yarp_install(TARGETS yarp_OpenNI2Device
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
endif()
