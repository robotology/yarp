# Copyright:(C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(stage TYPE StageControl INCLUDE StageControl.h)
  yarp_add_device_fingerprint(stage.ini stage)
endif()

if(NOT SKIP_stage)
  find_package(YARP REQUIRED)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
  find_package(Stage)

  if(Stage_FOUND)
    include_directories(${STAGE_INCLUDE_DIR})
    yarp_add_plugin(yarp_stage StageControl.cpp StageControl.h)
    target_link_libraries(yarp_stage ${STAGE_LIBRARY})
  else()
    message(STATUS "Could not find everything needed for stage device")
  endif()
endif()
