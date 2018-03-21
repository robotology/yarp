# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(portaudio
                    CATEGORY device
                    TYPE yarp::dev::PortAudioDeviceDriver
                    INCLUDE PortAudioDeviceDriver.h
                    EXTRA_CONFIG WRAPPER=grabber
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_HAS_PORTAUDIO")

if(NOT SKIP_portaudio)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  include_directories(SYSTEM ${PortAudio_INCLUDE_DIR})

  yarp_add_plugin(yarp_portaudio PortAudioBuffer.cpp
                                 PortAudioBuffer.h
                                 PortAudioDeviceDriver.cpp
                                 PortAudioDeviceDriver.h)
  target_link_libraries(yarp_portaudio YARP::YARP_OS
                                       YARP::YARP_sig
                                       YARP::YARP_dev
                                       ${PortAudio_LIBRARIES})
  yarp_install(TARGETS yarp_portaudio
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES portaudio.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
  yarp_install(FILES portaudio.xml
               DESTINATION ${YARP_MODULES_INSTALL_DIR})

  set_property(TARGET yarp_portaudio PROPERTY FOLDER "Plugins/Device")
endif()
