# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(KinectDeviceLocal TYPE yarp::dev::KinectDeviceDriverServer INCLUDE KinectYarpDeviceServerLib/KinectDeviceDriverServer.h)
  yarp_install(FILES kinect.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif(COMPILE_DEVICE_LIBRARY)

if(ENABLE_KinectDeviceLocal)
  add_subdirectory(KinectYarpDeviceServerLib)
endif()

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(KinectDeviceClient TYPE yarp::dev::KinectDeviceDriverClient INCLUDE KinectYarpDeviceClientLib/KinectDeviceDriverClient.h)
endif(COMPILE_DEVICE_LIBRARY)

if(ENABLE_KinectDeviceClient)
  add_subdirectory(KinectYarpDeviceClientLib)
endif()