# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(realsense2
                    CATEGORY device
                    TYPE yarp::dev::realsense2Driver
                    INCLUDE realsense2Driver.h
                    EXTRA_CONFIG WRAPPER=RGBDSensorWrapper
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_HAS_REALSENSE2")

if(ENABLE_realsense2)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  yarp_add_plugin(yarp_realsense2 realsense2Driver.cpp
                                 realsense2Driver.h)

  target_link_libraries(yarp_realsense2 YARP::YARP_OS
                                        YARP::YARP_sig
                                        YARP::YARP_dev
                                        realsense2::realsense2)

  yarp_install(TARGETS yarp_realsense2
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

  yarp_install(FILES realsense2.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_realsense2 PROPERTY FOLDER "Plugins/Device")
endif()
