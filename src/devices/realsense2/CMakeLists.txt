# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(realsense2
                    CATEGORY device
                    TYPE yarp::dev::realsense2Driver
                    INCLUDE realsense2Driver.h
                    EXTRA_CONFIG WRAPPER=RGBDSensorWrapper
                    DEPENDS "YARP_HAS_REALSENSE2")

if(ENABLE_realsense2)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  yarp_add_plugin(yarp_realsense2 realsense2Driver.cpp
                                  realsense2Driver.h)

  target_link_libraries(yarp_realsense2 PRIVATE YARP::YARP_OS
                                                YARP::YARP_sig
                                                YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_OS
                                                      YARP_sig
                                                      YARP_dev)

  target_link_libraries(yarp_realsense2 PRIVATE realsense2::realsense2)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS realsense2)

  yarp_install(TARGETS yarp_realsense2
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_realsense2 PROPERTY FOLDER "Plugins/Device")
endif()
