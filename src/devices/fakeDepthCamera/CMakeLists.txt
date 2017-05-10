# Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
# Authors: Andrea Ruzzenenti
# email:   andrea.ruzzenenti@iit.it
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


yarp_prepare_plugin(fakeDepthCamera
                    CATEGORY             device
                    TYPE                 yarp::dev::fakeDepthCameraDriver
                    INCLUDE              fakeDepthCameraDriver.h
                    EXTRA_CONFIG WRAPPER=RGBDSensorWrapper
                    DEPENDS              "CREATE_DEVICE_LIBRARY_MODULES")

if(ENABLE_fakeDepthCamera)

  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)

  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})

  yarp_add_plugin(yarp_fakeDepthCamera fakeDepthCameraDriver.cpp
                                   fakeDepthCameraDriver.h
                                   fakeDepthCameraImpl.hpp)

  target_link_libraries(yarp_fakeDepthCamera YARP_OS
                                             YARP_sig
                                             YARP_dev)

  yarp_install(TARGETS     yarp_fakeDepthCamera
               EXPORT      YARP
               COMPONENT   runtime
               LIBRARY     DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE     DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

  yarp_install(FILES       fakeDepthCamera.ini
               COMPONENT   runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_fakeDepthCamera PROPERTY FOLDER "Plugins/Device/Fake")
endif()
