FIND_PACKAGE(YARP REQUIRED)

IF (UNIX OR WIN32)

  IF (WIN32)
    SET(SYS winnt)
  ELSE (WIN32)
    SET(SYS linux)
  ENDIF (WIN32)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common)

  IF (COMPILE_DEVICE_LIBRARY)
    PREPARE_DEVICE(microphone TYPE MicrophoneDeviceDriver INCLUDE common/yarp/MicrophoneDeviceDriver.h WRAPPER grabber)
  ENDIF (COMPILE_DEVICE_LIBRARY)

  ADD_LIBRARY(microphone ${SYS}/yarp/MicrophoneDeviceDriver.cpp common/yarp/MicrophoneDeviceDriver.h)

ELSE (UNIX OR WIN32)

  MESSAGE(STATUS "not a system we know how to build a microphone driver on")

ENDIF (UNIX OR WIN32)


