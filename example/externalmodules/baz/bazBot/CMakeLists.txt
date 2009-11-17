# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

IF (COMPILE_DEVICE_LIBRARY)
  PREPARE_DEVICE(bazbot TYPE bazbot INCLUDE "bazbot.h")
ENDIF (COMPILE_DEVICE_LIBRARY)
 
IF (NOT SKIP_bazbot)
  FIND_PACKAGE(YARP REQUIRED)
  ADD_LIBRARY(bazbot bazbot.cpp bazbot.h)
ENDIF (NOT SKIP_bazbot)
