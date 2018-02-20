# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

yarp_prepare_plugin(KinectDeviceLocal
                    CATEGORY device
                    TYPE yarp::dev::KinectDeviceDriverServer
                    INCLUDE KinectYarpDeviceServerLib/KinectDeviceDriverServer.h
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_COMPILE_UNMAINTAINED"
                    ADVANCED)

if(ENABLE_KinectDeviceLocal)
  add_subdirectory(KinectYarpDeviceServerLib)
endif()

yarp_prepare_plugin(KinectDeviceClient
                    CATEGORY device
                    TYPE yarp::dev::KinectDeviceDriverClient
                    INCLUDE KinectYarpDeviceClientLib/KinectDeviceDriverClient.h
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_COMPILE_UNMAINTAINED"
                    ADVANCED)

if(ENABLE_KinectDeviceClient)
  add_subdirectory(KinectYarpDeviceClientLib)
endif()

if(ENABLE_KinectDeviceLocal OR ENABLE_KinectDeviceClient)
  yarp_install(FILES kinect.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()
