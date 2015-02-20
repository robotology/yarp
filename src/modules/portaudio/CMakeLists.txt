# Copyright:(C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(portaudio TYPE yarp::dev::PortAudioDeviceDriver INCLUDE PortAudioDeviceDriver.h WRAPPER grabber)
  yarp_install(FILES portaudio.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif(COMPILE_DEVICE_LIBRARY)

if(NOT SKIP_portaudio)
  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  link_libraries(${YARP_LIBRARIES})

  find_package(PortAudio)

  if(PortAudio_FOUND)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${PortAudio_INCLUDE_DIR})
    link_libraries(${PortAudio_LIBRARIES})
    yarp_add_plugin(yarp_portaudio PortAudioBuffer.cpp PortAudioBuffer.h PortAudioDeviceDriver.cpp PortAudioDeviceDriver.h)
    yarp_install(TARGETS yarp_portaudio
                 EXPORT YARP
                 COMPONENT runtime
                 LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  else(PortAudio_FOUND)
    message(SEND_ERROR "portaudio not found")
  endif(PortAudio_FOUND)
endif(NOT SKIP_portaudio)


