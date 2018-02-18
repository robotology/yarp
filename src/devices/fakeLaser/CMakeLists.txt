# Copyright (C) 2015 Istituto Italiano di Tecnologia (IIT)
# Author: Marco Randazzo
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(fakeLaser
                    CATEGORY device
                    TYPE FakeLaser
                    INCLUDE fakeLaser.h
                    DEPENDS CREATE_DEVICE_LIBRARY_MODULES)

if(NOT SKIP_fakeLaser)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  yarp_add_plugin(fakeLaser fakeLaser.h
                            fakeLaser.cpp)
  target_link_libraries(fakeLaser YARP::YARP_OS
                                  YARP::YARP_sig
                                  YARP::YARP_dev
                                  YARP::YARP_math)
  yarp_install(TARGETS fakeLaser
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES fakeLaser.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET fakeLaser PROPERTY FOLDER "Plugins/Device/Fake")
endif()
