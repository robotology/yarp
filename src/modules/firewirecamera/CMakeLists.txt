# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
IF (UNIX)

  IF (COMPILE_DEVICE_LIBRARY)
    YARP_PREPARE_DEVICE(firewirecamera TYPE FirewireCamera INCLUDE FirewireCamera.h WRAPPER grabber)
    YARP_ADD_DEVICE_FINGERPRINT(firewirecamera.ini firewirecamera)
  ENDIF (COMPILE_DEVICE_LIBRARY)

  FIND_PACKAGE(YARP REQUIRED)

  IF (NOT SKIP_firewirecamera)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${YARP_INCLUDE_DIRS})
    FIND_PACKAGE(YARP REQUIRED)
    MESSAGE(STATUS "Warning, just taking it on faith that that raw1394 and dc1394_control exist")
    LINK_LIBRARIES(dc1394_control ${YARP_LIBRARIES})
    YARP_ADD_PLUGIN(yarp_firewirecamera FirewireCamera.cpp FirewireCameraSet.cpp FirewireCameraSet.h dc1394settings.h FirewireCamera.h)

  ENDIF (NOT SKIP_firewirecamera)

ENDIF (UNIX)
