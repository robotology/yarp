# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

IF (COMPILE_DEVICE_LIBRARY)
  YARP_PREPARE_DEVICE(portaudio TYPE PortAudioDeviceDriver INCLUDE PortAudioDeviceDriver.h WRAPPER grabber)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_portaudio)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
LINK_LIBRARIES(${YARP_LIBRARIES})

INCLUDE(UsePkgConfig)
FIND_PACKAGE(PortAudio)

IF (PortAudio_FOUND)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${PortAudio_INCLUDE_DIR})
  LINK_LIBRARIES(${PortAudio_LIBRARIES})

  YARP_ADD_PLUGIN(yarp_portaudio PortAudioDeviceDriver.cpp PortAudioDeviceDriver.h)

ELSE (PortAudio_FOUND)

  MESSAGE(SEND_ERROR "portaudio not found")

ENDIF (PortAudio_FOUND)

ENDIF (NOT SKIP_portaudio)


