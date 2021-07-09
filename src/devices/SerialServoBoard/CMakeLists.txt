# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(SerialServoBoard
                    CATEGORY device
                    TYPE SerialServoBoard
                    INCLUDE "SerialServoBoard.h")

if(NOT SKIP_SerialServoBoard)
  yarp_add_plugin(yarp_SerialServoBoard)

  target_sources(yarp_SerialServoBoard PRIVATE SerialServoBoard.cpp
                                               SerialServoBoard.h)

  target_link_libraries(yarp_SerialServoBoard PRIVATE YARP::YARP_os
                                                      YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_dev)

  yarp_install(TARGETS yarp_SerialServoBoard
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_SerialServoBoard PROPERTY FOLDER "Unsupported")
endif()
