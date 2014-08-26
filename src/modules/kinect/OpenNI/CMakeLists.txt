# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(KinectDeviceLocal TYPE KinectDeviceDriverServer INCLUDE KinectYarpDeviceServerLib/KinectDeviceDriverServer.h)
  yarp_prepare_device(KinectDeviceClient TYPE KinectDeviceDriverClient INCLUDE KinectYarpDeviceClientLib/KinectDeviceDriverClient.h)
  yarp_add_device_fingerprint(kinect.ini KinectDeviceLocal KinectDeviceClient)
endif(COMPILE_DEVICE_LIBRARY)

if(ENABLE_KinectDeviceLocal OR ENABLE_KinectDeviceClient)
  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  link_libraries(${YARP_LIBRARIES})
endif()

if(ENABLE_KinectDeviceLocal)
  add_subdirectory (KinectYarpDeviceServerLib)
  set(CORELIBS ${YARP_LIBRARIES} YarpInterfaceLib)
endif()

if(ENABLE_KinectDeviceClient)
  add_subdirectory (KinectYarpDeviceClientLib)
endif()