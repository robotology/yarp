# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(localization2DServer
                    CATEGORY device
                    TYPE Localization2DServer
                    INCLUDE Localization2DServer.h
                    DEPENDS "TARGET YARP::YARP_math"
                    DEFAULT ON)

if(NOT SKIP_localization2DServer)
  yarp_add_plugin(yarp_localization2DServer)

  target_sources(yarp_localization2DServer PRIVATE Localization2DServer.h
                                                   Localization2DServer.cpp)

  target_link_libraries(yarp_localization2DServer YARP::YARP_os
                                                  YARP::YARP_sig
                                                  YARP::YARP_dev
                                                  YARP::YARP_rosmsg
                                                  YARP::YARP_math)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_rosmsg
                                                      YARP_math)

  yarp_install(TARGETS yarp_localization2DServer
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_localization2DServer PROPERTY FOLDER "Plugins/Device")
endif()

###########################################################################################################
yarp_prepare_plugin(localization2D_nws_yarp
                    CATEGORY device
                    TYPE Localization2D_nws_yarp
                    INCLUDE Localization2D_nws_yarp.h
                    DEPENDS "TARGET YARP::YARP_math"
                    DEFAULT ON)

if(NOT SKIP_localization2D_nws_yarp)
  yarp_add_plugin(yarp_localization2D_nws_yarp)

  target_sources(yarp_localization2D_nws_yarp PRIVATE Localization2D_nws_yarp.h
          Localization2D_nws_yarp.cpp)

  target_link_libraries(yarp_localization2D_nws_yarp YARP::YARP_os
                                                  YARP::YARP_sig
                                                  YARP::YARP_dev
                                                  YARP::YARP_math)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_math)

  yarp_install(TARGETS yarp_localization2D_nws_yarp
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_localization2D_nws_yarp PROPERTY FOLDER "Plugins/Device")
endif()

###########################################################################################################
yarp_prepare_plugin(localization2D_nws_ros
                    CATEGORY device
                    TYPE Localization2D_nws_ros
                    INCLUDE Localization2D_nws_ros.h
                    DEPENDS "TARGET YARP::YARP_math"
                    DEFAULT ON)

if(NOT SKIP_localization2D_nws_ros)
  yarp_add_plugin(yarp_localization2D_nws_ros)

  target_sources(yarp_localization2D_nws_ros PRIVATE Localization2D_nws_ros.h
          Localization2D_nws_ros.cpp)

  target_link_libraries(yarp_localization2D_nws_ros YARP::YARP_os
                                                  YARP::YARP_sig
                                                  YARP::YARP_dev
                                                  YARP::YARP_rosmsg
                                                  YARP::YARP_math)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_rosmsg
                                                      YARP_math)

  yarp_install(TARGETS yarp_localization2D_nws_ros
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_localization2D_nws_ros PROPERTY FOLDER "Plugins/Device")
endif()
