# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(controlboardwrapper2
                    CATEGORY device
                    TYPE ControlBoardWrapper
                    INCLUDE ControlBoardWrapper.h
                    EXTRA_CONFIG WRAPPER=controlboardwrapper2
                    DEFAULT ON)

if(NOT SKIP_controlboardwrapper2)
  yarp_add_plugin(yarp_controlboardwrapper2)

  target_sources(yarp_controlboardwrapper2 PRIVATE ControlBoardWrapper.cpp
                                                   ControlBoardWrapper.h
                                                   RPCMessagesParser.cpp
                                                   RPCMessagesParser.h
                                                   StreamingMessagesParser.cpp
                                                   StreamingMessagesParser.h
                                                   SubDevice.cpp
                                                   SubDevice.h)

  target_link_libraries(yarp_controlboardwrapper2 PRIVATE YARP::YARP_os
                                                          YARP::YARP_sig
                                                          YARP::YARP_dev
                                                          YARP::YARP_rosmsg)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_rosmsg)

  yarp_install(TARGETS yarp_controlboardwrapper2
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_controlboardwrapper2 PROPERTY FOLDER "Plugins/Device")
endif()
