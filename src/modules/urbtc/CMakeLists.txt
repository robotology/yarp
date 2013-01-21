# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
IF (COMPILE_DEVICE_LIBRARY)
  YARP_PREPARE_DEVICE(urbtc TYPE UrbtcControl INCLUDE UrbtcControl.h
		 WRAPPER controlboard)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_urbtc)

FIND_PACKAGE(YARP REQUIRED)

IF (UNIX)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

  ADD_LIBRARY(urbtc UrbtcControl.cpp UrbtcControl.h)

ELSE (UNIX)

  MESSAGE(SEND_ERROR "urbtc is for unix")

ENDIF (UNIX)

ENDIF (NOT SKIP_urbtc)

