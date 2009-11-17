# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

IF (COMPILE_DEVICE_LIBRARY)
  PREPARE_DEVICE(bazdevice TYPE bazdevice INCLUDE "bazdevice.h")
ENDIF (COMPILE_DEVICE_LIBRARY)
 
IF (NOT SKIP_bazdevice)
  FIND_PACKAGE(YARP REQUIRED)
  ADD_LIBRARY(bazdevice bazdevice.cpp bazdevice.h)
ENDIF (NOT SKIP_bazdevice)
