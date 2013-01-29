# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
IF (COMPILE_DEVICE_LIBRARY)
  YARP_PREPARE_DEVICE(dimax_u2c TYPE DimaxU2C INCLUDE DimaxU2C.h WRAPPER controlboard)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_dimax_u2c)

FIND_PACKAGE(YARP REQUIRED)

IF (WIN32)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

  LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/blob)
  LINK_LIBRARIES(U2CCommon I2cBrdg)
  YARP_ADD_PLUGIN(yarp_dimax_u2c DimaxU2C.cpp I2c.cpp Servo.cpp DimaxU2C.h Servo.h i2cbridge.h I2c.h u2c_common_func.h)


ELSE (WIN32)

  MESSAGE(FATAL_ERROR "not a system we know how to build dimax on yet")

ENDIF (WIN32)

ENDIF (NOT SKIP_dimax_u2c)

