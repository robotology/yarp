# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

############################################# frameTransformGet_nws_yarp
yarp_prepare_plugin(frameTransformGet_nws_yarp
                    CATEGORY device
                    TYPE FrameTransformGet_nws_yarp
                    INCLUDE frameTransformGet_nws_yarp.h
                    DEPENDS "TARGET YARP::YARP_math"
                    DEFAULT ON)

if(NOT SKIP_frameTransformGet_nws_yarp)
  yarp_add_plugin(yarp_frameTransformGet_nws_yarp)

  target_sources(yarp_frameTransformGet_nws_yarp PRIVATE frameTransformGet_nws_yarp.cpp
                                                  frameTransformGet_nws_yarp.h
                                                  $<TARGET_OBJECTS:frameTransformStorageMsgs>)

  target_include_directories(yarp_frameTransformGet_nws_yarp PRIVATE $<TARGET_PROPERTY:frameTransformStorageMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_frameTransformGet_nws_yarp PRIVATE YARP::YARP_os
                                                         YARP::YARP_dev)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_dev)

  yarp_install(TARGETS yarp_frameTransformGet_nws_yarp
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_frameTransformGet_nws_yarp PROPERTY FOLDER "Plugins/Device")
endif()
## YARP WRAPPER ##################################################################### END #


############################################# frameTransformGet_nwc_yarp
yarp_prepare_plugin(frameTransformGet_nwc_yarp
                    CATEGORY device
                    TYPE FrameTransformGet_nwc_yarp
                    INCLUDE frameTransformGet_nwc_yarp.h
                    DEPENDS "TARGET YARP::YARP_math"
                    DEFAULT ON)

if(NOT SKIP_frameTransformGet_nwc_yarp)
  yarp_add_plugin(yarp_frameTransformGet_nwc_yarp)

  target_sources(yarp_frameTransformGet_nwc_yarp PRIVATE frameTransformGet_nwc_yarp.cpp
                                                  frameTransformGet_nwc_yarp.h
                                                  $<TARGET_OBJECTS:frameTransformStorageMsgs>)

  target_include_directories(yarp_frameTransformGet_nwc_yarp PRIVATE $<TARGET_PROPERTY:frameTransformStorageMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_frameTransformGet_nwc_yarp PRIVATE YARP::YARP_os
                                                         YARP::YARP_dev)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_dev)

  yarp_install(TARGETS yarp_frameTransformGet_nwc_yarp
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_frameTransformGet_nwc_yarp PROPERTY FOLDER "Plugins/Device")
endif()
## YARP WRAPPER ##################################################################### END #

############################################# frameTransformGetMultiplexer
yarp_prepare_plugin(frameTransformGetMultiplexer
                    CATEGORY device
                    TYPE FrameTransformGetMultiplexer
                    INCLUDE frameTransformGetMultiplexer.h
                    DEPENDS "TARGET YARP::YARP_math"
                    DEFAULT ON)

if(NOT SKIP_frameTransformGetMultiplexer)
  yarp_add_plugin(yarp_frameTransformGetMultiplexer)

  target_sources(yarp_frameTransformGetMultiplexer PRIVATE frameTransformGetMultiplexer.cpp
                                                  frameTransformGetMultiplexer.h)


  target_link_libraries(yarp_frameTransformGetMultiplexer PRIVATE YARP::YARP_os
                                                         YARP::YARP_dev)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_dev)

  yarp_install(TARGETS yarp_frameTransformGetMultiplexer
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_frameTransformGetMultiplexer PROPERTY FOLDER "Plugins/Device")
endif()
## YARP WRAPPER #################################################################### END #

############################################################# frameTransformGet_nwc_ros #
yarp_prepare_plugin(frameTransformGet_nwc_ros
        CATEGORY device
        TYPE FrameTransformGet_nwc_ros
        INCLUDE frameTransformGet_nwc_ros.h
        DEPENDS "TARGET YARP::YARP_math"
        DEFAULT ON)

if(NOT SKIP_frameTransformGet_nwc_ros)
  yarp_add_plugin(yarp_frameTransformGet_nwc_ros)

  target_sources(yarp_frameTransformGet_nwc_ros PRIVATE frameTransformGet_nwc_ros.cpp
                                                        frameTransformGet_nwc_ros.h)
  target_sources(yarp_frameTransformGet_nwc_ros PRIVATE $<TARGET_OBJECTS:FrameTransformUtils>)
  target_include_directories(yarp_frameTransformGet_nwc_ros PRIVATE $<TARGET_PROPERTY:FrameTransformUtils,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_frameTransformGet_nwc_ros PRIVATE YARP::YARP_os
                                                               YARP::YARP_sig
                                                               YARP::YARP_dev
                                                               YARP::YARP_math
                                                               YARP::YARP_rosmsg)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_math
                                                      YARP_rosmsg)

  yarp_install(TARGETS yarp_frameTransformGet_nwc_ros
          EXPORT YARP_${YARP_PLUGIN_MASTER}
          COMPONENT ${YARP_PLUGIN_MASTER}
          LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
          ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
          YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_frameTransformGet_nwc_ros PROPERTY FOLDER "Plugins/Device")
endif()
## ROS WRAPPER #################################################################### END #
