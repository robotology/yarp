# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
    yarp_prepare_device(dimax_u2c TYPE yarp::dev::DimaxU2C INCLUDE DimaxU2C.h WRAPPER controlboard)
endif(COMPILE_DEVICE_LIBRARY)

if(NOT SKIP_dimax_u2c)

find_package(YARP REQUIRED)

if(WIN32)

    include_directories(${CMAKE_CURRENT_SOURCE_DIR})

    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/blob)
    link_libraries(U2CCommon I2cBrdg)
    yarp_add_plugin(yarp_dimax_u2c DimaxU2C.cpp I2c.cpp Servo.cpp DimaxU2C.h Servo.h i2cbridge.h I2c.h u2c_common_func.h)


else(WIN32)

    message(FATAL_ERROR "not a system we know how to build dimax on yet")

endif(WIN32)

endif(NOT SKIP_dimax_u2c)

