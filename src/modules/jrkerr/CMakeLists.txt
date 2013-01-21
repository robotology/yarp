# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
IF (COMPILE_DEVICE_LIBRARY)
  YARP_PREPARE_DEVICE(jrkerr TYPE JrkerrMotionControl INCLUDE JrkerrMotionControl.h WRAPPER controlboard)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_jrkerr)

FIND_PACKAGE(YARP REQUIRED)

IF (WIN32)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${YARP_INCLUDE_DIRS})

  ADD_LIBRARY(jrkerr JrkerrMotionControl.cpp nmccom.cpp picservo.cpp sio_util.cpp JrkerrMotionControl.h nmccom.h picservo.h sio_util.h)
  TARGET_LINK_LIBRARIES(jrkerr ${YARP_LIBRARIES})

ELSE (WIN32)

  MESSAGE(SEND_ERROR "not a system we know how to build jrkerr on yet, minor fixes needed")

ENDIF (WIN32)

ENDIF (NOT SKIP_jrkerr)

