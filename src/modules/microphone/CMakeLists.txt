# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
 
IF (COMPILE_DEVICE_LIBRARY)
  PREPARE_DEVICE(microphone TYPE MicrophoneDeviceDriver INCLUDE MicrophoneDeviceDriver.h WRAPPER grabber)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_microphone)

FIND_PACKAGE(YARP REQUIRED)

IF (UNIX OR WIN32)

  IF (WIN32)
    SET(SYS Windows)
  ELSE (WIN32)
    SET(SYS Linux)
  ENDIF (WIN32)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

  ADD_LIBRARY(microphone ${SYS}MicrophoneDeviceDriver.cpp MicrophoneDeviceDriver.h)

ELSE (UNIX OR WIN32)

  MESSAGE(SEND_ERROR "not a system we know how to build a microphone driver on")

ENDIF (UNIX OR WIN32)

ENDIF (NOT SKIP_microphone)
