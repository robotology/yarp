# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
IF (COMPILE_DEVICE_LIBRARY)
	YARP_PREPARE_DEVICE(KinectDeviceLocal TYPE KinectDeviceDriverServer INCLUDE KinectYarpDeviceServerLib/KinectDeviceDriverServer.h)
	YARP_PREPARE_DEVICE(KinectDeviceClient TYPE KinectDeviceDriverClient INCLUDE KinectYarpDeviceClientLib/KinectDeviceDriverClient.h)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (ENABLE_KinectDeviceLocal OR ENABLE_KinectDeviceClient)
  FIND_PACKAGE(YARP REQUIRED)
  INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
#  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/IKinectDevice)
  LINK_LIBRARIES(${YARP_LIBRARIES})
  #add_subdirectory (lib) 
ENDIF()

IF (ENABLE_KinectDeviceLocal)
  add_subdirectory (KinectYarpDeviceServerLib)
  SET(CORELIBS ${YARP_LIBRARIES} YarpInterfaceLib)
ENDIF()

IF (ENABLE_KinectDeviceClient)  
  add_subdirectory (KinectYarpDeviceClientLib)
ENDIF()