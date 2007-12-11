FIND_PACKAGE(YARP REQUIRED)


INCLUDE(UsePkgConfig)
FIND_PACKAGE(PortAudio)

IF (PortAudio_FOUND)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common)
  LINK_LIBRARIES(${PortAudio_LIBRARIES})

  IF (COMPILE_DEVICE_LIBRARY)
    PREPARE_DEVICE(portaudio TYPE PortAudioDeviceDriver INCLUDE common/yarp/PortAudioDeviceDriver.h WRAPPER grabber)
  ENDIF (COMPILE_DEVICE_LIBRARY)

  ADD_LIBRARY(portaudio default/yarp/PortAudioDeviceDriver.cpp common/yarp/PortAudioDeviceDriver.h)

ELSE (PortAudio_FOUND)

  MESSAGE(STATUS "portaudio not found")

ENDIF (PortAudio_FOUND)



