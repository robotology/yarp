# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.6.4)

IF (COMPILE_DEVICE_LIBRARY)
  PREPARE_DEVICE(bazdevice TYPE bazdevice INCLUDE "bazdevice.h")
ENDIF (COMPILE_DEVICE_LIBRARY)
 
IF (NOT SKIP_bazdevice)
  FIND_PACKAGE(YARP REQUIRED)
  INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
  LINK_LIBRARIES(${YARP_LIBRARIES})
  ADD_LIBRARY(bazdevice bazdevice.cpp bazdevice.h)
ENDIF (NOT SKIP_bazdevice)
