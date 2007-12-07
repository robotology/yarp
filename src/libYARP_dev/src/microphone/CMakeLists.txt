FIND_PACKAGE(YARP REQUIRED)

IF (UNIX OR WINDOWS)

  IF (WINDOWS)
    SET(SYS winnt)
  ELSE (WINDOWS)
    SET(SYS linux)
  ENDIF (WINDOWS)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common)

  IF (COMPILE_DEVICE_LIBRARY)
    PREPARE_DEVICE(microphone TYPE MicrophoneDeviceDriver INCLUDE common/yarp/MicrophoneDeviceDriver.h WRAPPER grabber)
  ENDIF (COMPILE_DEVICE_LIBRARY)

  ADD_LIBRARY(microphone ${SYS}/yarp/MicrophoneDeviceDriver.cpp common/yarp/MicrophoneDeviceDriver.h)

ELSE (UNIX OR WINDOWS)

  MESSAGE(STATUS "not a system we know how to build a microphone driver on")

ENDIF (UNIX OR WINDOWS)


