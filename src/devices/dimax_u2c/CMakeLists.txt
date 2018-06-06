# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(dimax_u2c
                    CATEGORY device
                    TYPE yarp::dev::DimaxU2C
                    INCLUDE DimaxU2C.h
                    EXTRA_CONFIG WRAPPER=controlboard
                    DEPENDS "WIN32")

if(NOT SKIP_dimax_u2c)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  link_directories(${CMAKE_CURRENT_SOURCE_DIR}/blob)
  yarp_add_plugin(yarp_dimax_u2c DimaxU2C.cpp
                                 I2c.cpp
                                 Servo.cpp
                                 DimaxU2C.h
                                 Servo.h
                                 i2cbridge.h
                                 I2c.h
                                 u2c_common_func.h)
  target_link_libraries(yarp_dimax_u2c YARP_OS
                                       YARP_sig
                                       YARP_dev
                                       U2CCommon
                                       I2cBrdg)
  yarp_install(TARGETS yarp_dimax_u2c
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

  set_property(TARGET yarp_dimax_u2c PROPERTY FOLDER "Unsupported")
endif()
