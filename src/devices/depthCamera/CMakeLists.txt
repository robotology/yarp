# Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
# Authors: Alberto Cardellino, Andrea Ruzzenenti
# email:   andrea.ruzzenenti@iit.it
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


yarp_prepare_plugin(depthCamera
                    CATEGORY device
                    TYPE yarp::dev::depthCameraDriver
                    INCLUDE depthCameraDriver.h
                    EXTRA_CONFIG WRAPPER=RGBDSensorWrapper
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_HAS_OpenNI2")

if(ENABLE_depthCamera)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  include_directories(SYSTEM ${OpenNI2_INCLUDE_DIRS})

  yarp_add_plugin(yarp_depthCamera depthCameraDriver.cpp
                                   depthCameraDriver.h
                                   depthCameraImpl.hpp)

  target_link_libraries(yarp_depthCamera YARP::YARP_OS
                                         YARP::YARP_sig
                                         YARP::YARP_dev
                                         ${OpenNI2_LIBRARIES})

  yarp_install(TARGETS yarp_depthCamera
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

  yarp_install(FILES depthCamera.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_depthCamera PROPERTY FOLDER "Plugins/Device")
endif()
