FIND_PACKAGE(YARP REQUIRED)

IF (WIN32)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common)

  IF (COMPILE_DEVICE_LIBRARY)
    PREPARE_DEVICE(dimax_u2c TYPE DimaxU2C INCLUDE common/yarp/DimaxU2C.h WRAPPER controlboard)
  ENDIF (COMPILE_DEVICE_LIBRARY)

  LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/winnt/dd_orig/lib)
  LINK_LIBRARIES(U2CCommon I2cBrdg)
  ADD_LIBRARY(dimax_u2c winnt/yarp/DimaxU2C.cpp winnt/yarp/I2c.cpp winnt/yarp/Servo.cpp common/yarp/DimaxU2C.h common/yarp/Servo.h winnt/dd_orig/include/i2cbridge.h winnt/dd_orig/include/I2c.h winnt/dd_orig/include/u2c_common_func.h)


ELSE (WIN32)

  MESSAGE(STATUS "not a system we know how to build dimax on yet")

ENDIF (WIN32)


