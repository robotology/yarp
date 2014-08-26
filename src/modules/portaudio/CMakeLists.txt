# Copyright:(C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(portaudio TYPE PortAudioDeviceDriver INCLUDE PortAudioDeviceDriver.h WRAPPER grabber)
  yarp_add_device_fingerprint(portaudio.ini portaudio)
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
  else(PortAudio_FOUND)
    message(SEND_ERROR "portaudio not found")
  endif(PortAudio_FOUND)
endif(NOT SKIP_portaudio)


