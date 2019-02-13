# Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(audioPlayerWrapper
                    CATEGORY device
                    TYPE yarp::dev::audioPlayerWrapper
                    INCLUDE audioPlayerWrapper.h
                    EXTRA_CONFIG WRAPPER=audioPlayerWrapper
                    DEFAULT ON)

if(NOT SKIP_audioPlayerWrapper)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  yarp_add_plugin(yarp_audioPlayerWrapper audioPlayerWrapper.cpp
                                            audioPlayerWrapper.h)
  target_link_libraries(yarp_audioPlayerWrapper PRIVATE YARP::YARP_OS
                                                          YARP::YARP_sig
                                                          YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_OS
                                                      YARP_sig
                                                      YARP_dev)

  yarp_install(TARGETS yarp_audioPlayerWrapper
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES audioPlayerWrapper.ini
               COMPONENT ${YARP_PLUGIN_MASTER}
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_audioPlayerWrapper PROPERTY FOLDER "Plugins/Device")
endif()
