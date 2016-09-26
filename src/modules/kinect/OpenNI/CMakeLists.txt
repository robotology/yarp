# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

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
