# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
IF (COMPILE_DEVICE_LIBRARY)
  YARP_PREPARE_DEVICE(stage TYPE StageControl INCLUDE StageControl.h)
  YARP_ADD_DEVICE_FINGERPRINT(stage.ini stage)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_stage)

  FIND_PACKAGE(YARP REQUIRED)
  INCLUDE(UsePkgConfig)

  SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

  FIND_PACKAGE(Stage)

  IF (Stage_FOUND)

  INCLUDE_DIRECTORIES(${STAGE_INCLUDE_DIR})
  LINK_LIBRARIES(${STAGE_LIBRARY})

  YARP_ADD_PLUGIN(yarp_stage StageControl.cpp StageControl.h)

  ELSE (Stage_FOUND)

  MESSAGE(STATUS "Could not find everything needed for stage device")

  ENDIF (Stage_FOUND)

ENDIF (NOT SKIP_stage)

