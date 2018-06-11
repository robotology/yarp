# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(fakeAnalogSensor
                    CATEGORY device
                    TYPE yarp::dev::FakeAnalogSensor
                    INCLUDE fakeAnalogSensor.h
                    EXTRA_CONFIG WRAPPER=analogServer)

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
