# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(dimax_u2c
                    CATEGORY device
                    TYPE yarp::dev::DimaxU2C
                    INCLUDE DimaxU2C.h
                    EXTRA_CONFIG WRAPPER=controlboard
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;WIN32")

if(NOT SKIP_dimax_u2c)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})
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
endif()
