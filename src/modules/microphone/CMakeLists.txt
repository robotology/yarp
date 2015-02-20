# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
IF (COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(microphone TYPE yarp::dev::MicrophoneDeviceDriver INCLUDE MicrophoneDeviceDriver.h WRAPPER grabber)
  yarp_install(FILES microphone.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
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
  INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
  LINK_LIBRARIES(${YARP_LIBRARIES})

  YARP_ADD_PLUGIN(yarp_microphone ${SYS}MicrophoneDeviceDriver.cpp MicrophoneDeviceDriver.h)
  yarp_install(TARGETS yarp_microphone
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

ELSE (UNIX OR WIN32)

  MESSAGE(SEND_ERROR "not a system we know how to build a microphone driver on")

ENDIF (UNIX OR WIN32)

ENDIF (NOT SKIP_microphone)
