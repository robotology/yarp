# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2013 Konstantinos Theofilis <k.theofilis@herts.ac.uk>
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

find_path(NITE2_INCLUDE_LOCAL NAMES NiTE.h HINTS $ENV{NITE2_INCLUDE})
find_library(NITE2_LIBRARY NAMES NiTE2 HINTS $ENV{NITE2_REDIST64})
if(NITE2_INCLUDE_LOCAL)
  set(NITE2_FOUND TRUE)
else()
  set(NITE2_FOUND FALSE)
endif()


yarp_prepare_plugin(OpenNI2DeviceServer
                    CATEGORY device
                    TYPE yarp::dev::OpenNI2DeviceDriverServer
                    INCLUDE OpenNI2DeviceDriverServer.h
                    DEPENDS "YARP_HAS_MATH_LIB;OpenNI2_FOUND")
yarp_prepare_plugin(OpenNI2DeviceClient
                    CATEGORY device
                    TYPE yarp::dev::OpenNI2DeviceDriverClient
                    INCLUDE OpenNI2DeviceDriverClient.h
                    DEPENDS "YARP_HAS_MATH_LIB;OpenNI2_FOUND")

if (NOT SKIP_OpenNI2DeviceServer OR NOT SKIP_OpenNI2DeviceClient)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  if(NOT NITE2_FOUND)
    message(STATUS "OpenNI2Device: NITE2 library was not found, skeleton extraction feature will be skipped by driver. "
                   "If you need this feature please install NITE2 library.")
  else()
    add_definitions(-DOPENNI2_DRIVER_USES_NITE2)
  endif()

  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

  include_directories(SYSTEM ${OpenNI2_INCLUDE_DIRS})
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
  target_link_libraries(yarp_OpenNI2Device YARP::YARP_OS
                                           YARP::YARP_sig
                                           YARP::YARP_dev
                                           YARP::YARP_math
                                           ${OpenNI2_LIBRARIES})

  if(NITE2_FOUND)
    target_link_libraries(yarp_OpenNI2Device ${NITE2_LIBRARY})
  endif()

  yarp_install(TARGETS yarp_OpenNI2Device
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES openni2.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_OpenNI2Device PROPERTY FOLDER "Unsupported")
endif()
