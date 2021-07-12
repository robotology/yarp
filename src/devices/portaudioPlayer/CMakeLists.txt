# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(portaudioPlayer
                    CATEGORY device
                    TYPE PortAudioPlayerDeviceDriver
                    INCLUDE PortAudioPlayerDeviceDriver.h
                    EXTRA_CONFIG WRAPPER=AudioPlayerWrapper
                    DEPENDS "YARP_HAS_PortAudio")

if(NOT SKIP_portaudioPlayer)
  yarp_add_plugin(yarp_portaudioPlayer)

  target_sources(yarp_portaudioPlayer PRIVATE PortAudioPlayerDeviceDriver.cpp
                                              PortAudioPlayerDeviceDriver.h)

  target_link_libraries(yarp_portaudioPlayer PRIVATE YARP::YARP_os
                                                     YARP::YARP_sig
                                                     YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  target_include_directories(yarp_portaudioPlayer SYSTEM PRIVATE ${PortAudio_INCLUDE_DIRS})
  target_link_libraries(yarp_portaudioPlayer PRIVATE ${PortAudio_LIBRARIES})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS PortAudio) (not using targets)

  yarp_install(TARGETS yarp_portaudioPlayer
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_portaudioPlayer PROPERTY FOLDER "Plugins/Device")
endif()
