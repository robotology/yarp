# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(depthCamera
                    CATEGORY device
                    TYPE yarp::dev::depthCameraDriver
                    INCLUDE depthCameraDriver.h
                    EXTRA_CONFIG WRAPPER=RGBDSensorWrapper
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_HAS_OPENNI2")

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
