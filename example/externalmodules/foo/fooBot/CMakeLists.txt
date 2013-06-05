# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.7)

IF (COMPILE_DEVICE_LIBRARY)
  YARP_PREPARE_DEVICE(foobot TYPE foobot INCLUDE "foobot.h")
ENDIF (COMPILE_DEVICE_LIBRARY)
 
IF (NOT SKIP_foobot)
  FIND_PACKAGE(YARP REQUIRED)
  INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
  LINK_LIBRARIES(${YARP_LIBRARIES})
  ADD_LIBRARY(foobot foobot.cpp foobot.h)
ENDIF (NOT SKIP_foobot)
