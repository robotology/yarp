# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(controlboardremapper
                    CATEGORY device
                    TYPE ControlBoardRemapper
                    INCLUDE ControlBoardRemapper.h
                    EXTRA_CONFIG WRAPPER=controlboardwrapper2
                    DEFAULT ON)
yarp_prepare_plugin(remotecontrolboardremapper
                    CATEGORY device
                    TYPE RemoteControlBoardRemapper
                    INCLUDE RemoteControlBoardRemapper.h
                    EXTRA_CONFIG WRAPPER=controlboardwrapper2
                    DEFAULT ON)

if(NOT SKIP_controlboardremapper OR NOT SKIP_remotecontrolboardremapper)
  yarp_add_plugin(yarp_controlboardremapper)

  target_sources(yarp_controlboardremapper PRIVATE ControlBoardRemapper.cpp
                                                   ControlBoardRemapper.h
                                                   ControlBoardRemapperHelpers.cpp
                                                   ControlBoardRemapperHelpers.h
                                                   ControlBoardRemapperLogComponent.cpp
                                                   ControlBoardRemapperLogComponent.h
                                                   RemoteControlBoardRemapper.cpp
                                                   RemoteControlBoardRemapper.h)

  target_link_libraries(yarp_controlboardremapper PRIVATE YARP::YARP_os
                                                          YARP::YARP_sig
                                                          YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  yarp_install(TARGETS yarp_controlboardremapper
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_controlboardremapper PROPERTY FOLDER "Plugins/Device")
endif()
