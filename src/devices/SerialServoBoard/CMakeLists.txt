# Copyright: (C) 2009 RobotCub Consortium
# Authors: Giacomo Spigler, Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(SerialServoBoard
                    CATEGORY device
                    TYPE SerialServoBoard
                    INCLUDE "SerialServoBoard.h"
                    DEPENDS CREATE_DEVICE_LIBRARY_MODULES)

if(NOT SKIP_SerialServoBoard)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})
  yarp_add_plugin(yarp_SerialServoBoard SerialServoBoard.cpp
                                        SerialServoBoard.h)
  target_link_libraries(yarp_SerialServoBoard YARP_OS
                                          YARP_dev)
  yarp_install(TARGETS yarp_SerialServoBoard
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES SerialServoBoard.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()

