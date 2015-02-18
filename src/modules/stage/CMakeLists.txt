# Copyright:(C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(stage TYPE yarp::dev::StageControl INCLUDE StageControl.h)
  yarp_install(FILES stage.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()

if(NOT SKIP_stage)
  find_package(YARP REQUIRED)
  find_package(Stage)

  if(Stage_FOUND)
    include_directories(${STAGE_INCLUDE_DIR})
    yarp_add_plugin(yarp_stage StageControl.cpp StageControl.h)
    target_link_libraries(yarp_stage ${STAGE_LIBRARY})
  else()
    message(STATUS "Could not find everything needed for stage device")
  endif()
endif()
