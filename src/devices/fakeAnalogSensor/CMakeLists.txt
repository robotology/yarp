# Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
# Authors: Alberto Cardellino
# email:   alberto.cardellino@iit.it
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT



yarp_prepare_plugin(fakeAnalogSensor
                    CATEGORY device
                    TYPE yarp::dev::FakeAnalogSensor
                    INCLUDE fakeAnalogSensor.h
                    EXTRA_CONFIG WRAPPER=analogServer
                    DEPENDS CREATE_DEVICE_LIBRARY_MODULES)

if(ENABLE_fakeAnalogSensor)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})
  yarp_add_plugin(yarp_fakeAnalogSensor fakeAnalogSensor.cpp
                                        fakeAnalogSensor.h)
  target_link_libraries(yarp_fakeAnalogSensor YARP_OS
                                              YARP_sig
                                              YARP_dev)
  yarp_install(TARGETS yarp_fakeAnalogSensor
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES fakeAnalogSensor.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()
