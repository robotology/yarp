# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(SerialServoBoard
                    CATEGORY device
                    TYPE SerialServoBoard
                    INCLUDE "SerialServoBoard.h")

if(NOT SKIP_SerialServoBoard)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  yarp_add_plugin(yarp_SerialServoBoard SerialServoBoard.cpp
                                        SerialServoBoard.h)
  target_link_libraries(yarp_SerialServoBoard YARP::YARP_OS
                                              YARP::YARP_dev)
  yarp_install(TARGETS yarp_SerialServoBoard
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES SerialServoBoard.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_SerialServoBoard PROPERTY FOLDER "Unsupported")
endif()

