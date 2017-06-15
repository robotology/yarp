# Copyright (C) 2010 Marco Randazzo
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(laserHokuyo
                    CATEGORY device
                    TYPE laserHokuyo
                    INCLUDE laserHokuyo.h
                    DEPENDS CREATE_DEVICE_LIBRARY_MODULES)

if(NOT SKIP_laserHokuyo)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  yarp_add_plugin(laserHokuyo laserHokuyo.h
                              laserHokuyo.cpp)
  target_link_libraries(laserHokuyo YARP::YARP_OS
                                    YARP::YARP_sig
                                    YARP::YARP_dev)
  yarp_install(TARGETS laserHokuyo
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES laserHokuyo.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET laserHokuyo PROPERTY FOLDER "Plugins/Device")
endif()
