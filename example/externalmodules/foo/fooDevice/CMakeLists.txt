# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

IF (COMPILE_DEVICE_LIBRARY)
  PREPARE_DEVICE(foodevice TYPE foodevice INCLUDE "foodevice.h")
ENDIF (COMPILE_DEVICE_LIBRARY)
 
IF (NOT SKIP_foodevice)
  FIND_PACKAGE(YARP REQUIRED)
  ADD_LIBRARY(foodevice foodevice.cpp foodevice.h)
ENDIF (NOT SKIP_foodevice)
