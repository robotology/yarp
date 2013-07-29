# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
IF (COMPILE_DEVICE_LIBRARY)
  YARP_PREPARE_DEVICE(serial TYPE ServerSerial INCLUDE yarp/dev/ServerSerial.h
			  WRAPPER serial)
  YARP_PREPARE_DEVICE(serialport TYPE SerialDeviceDriver INCLUDE SerialDeviceDriver.h WRAPPER serial)
  YARP_ADD_DEVICE_FINGERPRINT(serial.ini serial serialport)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (SKIP_serial AND SKIP_serialport)
  SET(SKIP_all_serial TRUE)
ENDIF (SKIP_serial AND SKIP_serialport)


IF (NOT SKIP_all_serial)

FIND_PACKAGE(YARP REQUIRED)
FIND_PACKAGE(ACE)

IF (ACE_FOUND)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${YARP_INCLUDE_DIRS} ${ACE_INCLUDE_DIRS})
  ADD_DEFINITIONS(${YARP_DEFINITIONS})

  YARP_ADD_PLUGIN(yarp_serial SerialDeviceDriver.cpp SerialDeviceDriver.h)
  target_link_libraries(yarp_serial YARP_OS YARP_dev)
  target_link_libraries(yarp_serial ${ACE_LIBRARIES})

ELSE (ACE_FOUND)

  MESSAGE(SEND_ERROR "ACE not found; needed by serial driver")

ENDIF (ACE_FOUND)

ENDIF (NOT SKIP_all_serial)

