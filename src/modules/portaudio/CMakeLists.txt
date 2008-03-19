IF (COMPILE_DEVICE_LIBRARY)
  PREPARE_DEVICE(portaudio TYPE PortAudioDeviceDriver INCLUDE PortAudioDeviceDriver.h WRAPPER grabber)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_portaudio)

FIND_PACKAGE(YARP REQUIRED)

INCLUDE(UsePkgConfig)
FIND_PACKAGE(PortAudio)

IF (PortAudio_FOUND)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${PortAudio_INCLUDE_DIR})
  LINK_LIBRARIES(${PortAudio_LIBRARIES})

  ADD_LIBRARY(portaudio PortAudioDeviceDriver.cpp PortAudioDeviceDriver.h)

ELSE (PortAudio_FOUND)

  MESSAGE(SEND_ERROR "portaudio not found")

ENDIF (PortAudio_FOUND)

ENDIF (NOT SKIP_portaudio)


