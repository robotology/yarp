# Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
# Authors: Andrea Ruzzenenti
# email:   andrea.ruzzenenti@iit.it
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


yarp_prepare_plugin(fakeDepthCamera
                    CATEGORY device
                    TYPE yarp::dev::fakeDepthCameraDriver
                    INCLUDE fakeDepthCameraDriver.h
                    EXTRA_CONFIG WRAPPER=RGBDSensorWrapper
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES")

if(ENABLE_fakeDepthCamera)

  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  yarp_add_plugin(yarp_fakeDepthCamera fakeDepthCameraDriver.cpp
                                   fakeDepthCameraDriver.h
                                   fakeDepthCameraImpl.hpp)

  target_link_libraries(yarp_fakeDepthCamera YARP::YARP_OS
                                             YARP::YARP_sig
                                             YARP::YARP_dev)

  yarp_install(TARGETS yarp_fakeDepthCamera
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

  yarp_install(FILES fakeDepthCamera.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_fakeDepthCamera PROPERTY FOLDER "Plugins/Device/Fake")
endif()
