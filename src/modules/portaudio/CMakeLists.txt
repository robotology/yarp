# Copyright:(C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(portaudio
                    CATEGORY device
                    TYPE yarp::dev::PortAudioDeviceDriver
                    INCLUDE PortAudioDeviceDriver.h
                    EXTRA_CONFIG WRAPPER=grabber
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_HAS_PORTAUDIO")

if(NOT SKIP_portaudio)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})
  include_directories(SYSTEM ${PortAudio_INCLUDE_DIR})

  yarp_add_plugin(yarp_portaudio PortAudioBuffer.cpp
                                 PortAudioBuffer.h
                                 PortAudioDeviceDriver.cpp
                                 PortAudioDeviceDriver.h)
  target_link_libraries(yarp_portaudio YARP_OS
                                       YARP_sig
                                       YARP_dev
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
endif()
