FIND_PACKAGE(YARP REQUIRED)
FIND_PACKAGE(Ace REQUIRED)

IF (Ace_FOUND)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common ${ACE_INCLUDE_DIR})
  LINK_LIBRARIES(${ACE_LIBRARY})

  IF (COMPILE_DEVICE_LIBRARY)
    PREPARE_DEVICE(serial TYPE ServerSerial INCLUDE yarp/dev/ServerSerial.h)
    PREPARE_DEVICE(serialport TYPE SerialDeviceDriver INCLUDE common/yarp/SerialDeviceDriver.h WRAPPER serial)
  ENDIF (COMPILE_DEVICE_LIBRARY)

  ADD_LIBRARY(serial default/yarp/SerialDeviceDriver.cpp common/yarp/SerialDeviceDriver.h)

ELSE (Ace_FOUND)

  MESSAGE(STATUS "ACE not found")

ENDIF (Ace_FOUND)


