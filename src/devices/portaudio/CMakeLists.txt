# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(portaudio
                    CATEGORY device
                    TYPE PortAudioDeviceDriver
                    INCLUDE PortAudioDeviceDriver.h
                    EXTRA_CONFIG WRAPPER=grabber
                    DEFAULT OFF
                    DEPENDS "NOT YARP_NO_DEPRECATED;YARP_HAS_PortAudio") # DEPRECATED Since YARP 3.2

if(NOT SKIP_portaudio)
  yarp_add_plugin(yarp_portaudio)

  target_sources(yarp_portaudio PRIVATE PortAudioBuffer.cpp
                                        PortAudioBuffer.h
                                        PortAudioDeviceDriver.cpp
                                        PortAudioDeviceDriver.h)

  target_link_libraries(yarp_portaudio PRIVATE YARP::YARP_os
                                               YARP::YARP_sig
                                               YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  target_include_directories(yarp_portaudio SYSTEM PRIVATE ${PortAudio_INCLUDE_DIRS})
  target_link_libraries(yarp_portaudio PRIVATE ${PortAudio_LIBRARIES})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS PortAudio) (not using targets)

  yarp_install(TARGETS yarp_portaudio
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
  yarp_install(FILES portaudio.xml
               DESTINATION ${YARP_MODULES_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_portaudio PROPERTY FOLDER "Plugins/Device")
endif()
