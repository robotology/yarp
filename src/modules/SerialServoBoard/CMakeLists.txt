# Copyright: (C) 2009 RobotCub Consortium
# Authors: Giacomo Spigler, Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
IF (COMPILE_DEVICE_LIBRARY)
  YARP_PREPARE_DEVICE(SerialServoBoard TYPE SerialServoBoard INCLUDE "SerialServoBoard.h")
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_SerialServoBoard)


  FIND_PACKAGE(YARP REQUIRED)

  INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
  yarp_add_plugin(ySerialServoBoard SerialServoBoard.cpp SerialServoBoard.h)
  TARGET_LINK_LIBRARIES(ySerialServoBoard ${YARP_LIBRARIES})


ENDIF (NOT SKIP_SerialServoBoard)

