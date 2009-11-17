# Copyright: (C) 2009 RobotCub Consortium
# Authors: Giacomo Spigler, Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
 
IF (COMPILE_DEVICE_LIBRARY)
  PREPARE_DEVICE(SerialServoBoard TYPE SerialServoBoard INCLUDE "SerialServoBoard.h")
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_SerialServoBoard)


  FIND_PACKAGE(YARP REQUIRED)

  ADD_LIBRARY(ySerialServoBoard SerialServoBoard.cpp SerialServoBoard.h)


ENDIF (NOT SKIP_SerialServoBoard)

