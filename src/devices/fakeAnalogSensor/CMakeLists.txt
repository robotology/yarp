# Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
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
  yarp_add_plugin(yarp_fakeAnalogSensor fakeAnalogSensor.cpp
                                        fakeAnalogSensor.h)
  target_link_libraries(yarp_fakeAnalogSensor YARP::YARP_OS
                                              YARP::YARP_sig
                                              YARP::YARP_dev)
  yarp_install(TARGETS yarp_fakeAnalogSensor
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES fakeAnalogSensor.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_fakeAnalogSensor PROPERTY FOLDER "Plugins/Device/Fake")
endif()
