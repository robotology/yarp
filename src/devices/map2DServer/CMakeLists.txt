# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(map2DServer
                    CATEGORY device
                    TYPE Map2DServer
                    INCLUDE Map2DServer.h
                    DEPENDS "TARGET YARP::YARP_math"
                    DEFAULT ON)

if(NOT SKIP_map2DServer)
  yarp_add_plugin(yarp_map2DServer)

  target_sources(yarp_map2DServer PRIVATE Map2DServer.cpp
                                          Map2DServer.h)

  target_link_libraries(yarp_map2DServer PRIVATE YARP::YARP_os
                                                 YARP::YARP_sig
                                                 YARP::YARP_dev
                                                 YARP::YARP_math
                                                 YARP::YARP_rosmsg)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_math
                                                      YARP_rosmsg)

  yarp_install(TARGETS yarp_map2DServer
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_map2DServer PROPERTY FOLDER "Plugins/Device")
endif()

############################################################################################################################

yarp_prepare_plugin(map2D_nws_yarp
                    CATEGORY device
                    TYPE Map2D_nws_yarp
                    INCLUDE Map2D_nws_yarp.h
                    DEPENDS "TARGET YARP::YARP_math"
                    DEFAULT ON)

if(NOT SKIP_map2D_nws_yarp)
  yarp_add_plugin(yarp_map2D_nws_yarp)

  target_sources(yarp_map2D_nws_yarp PRIVATE Map2D_nws_yarp.cpp
          Map2D_nws_yarp.h)

  target_link_libraries(yarp_map2D_nws_yarp PRIVATE YARP::YARP_os
                                                 YARP::YARP_sig
                                                 YARP::YARP_dev
                                                 YARP::YARP_math
                                                 YARP::YARP_rosmsg)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_math
                                                      YARP_rosmsg)

  yarp_install(TARGETS yarp_map2D_nws_yarp
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_map2D_nws_yarp PROPERTY FOLDER "Plugins/Device")
endif()

############################################################################################################################

yarp_prepare_plugin(map2D_nws_ros
                    CATEGORY device
                    TYPE Map2D_nws_ros
                    INCLUDE Map2D_nws_ros.h
                    DEPENDS "TARGET YARP::YARP_math"
                    DEFAULT ON)

if(NOT SKIP_map2D_nws_ros)
  yarp_add_plugin(yarp_map2D_nws_ros)

  target_sources(yarp_map2D_nws_ros PRIVATE Map2D_nws_ros.cpp
          Map2D_nws_ros.h)

  target_link_libraries(yarp_map2D_nws_ros PRIVATE YARP::YARP_os
                                                 YARP::YARP_sig
                                                 YARP::YARP_dev
                                                 YARP::YARP_math
                                                 YARP::YARP_rosmsg)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_math
                                                      YARP_rosmsg)

  yarp_install(TARGETS yarp_map2D_nws_ros
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_map2D_nws_ros PROPERTY FOLDER "Plugins/Device")
endif()
