# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(navigation2DServer
  CATEGORY device
  TYPE navigation2DServer
  INCLUDE navigation2DServer.h
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(NOT SKIP_navigation2DServer)
  yarp_add_plugin(yarp_navigation2DServer)

  target_sources(yarp_navigation2DServer
    PRIVATE
      navigation2DServer.h
      navigation2DServer.cpp
  )

  target_link_libraries(yarp_navigation2DServer
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      YARP::YARP_math
      YARP::YARP_rosmsg
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
    YARP_math
    YARP_rosmsg
  )

  yarp_install(
    TARGETS yarp_navigation2DServer
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_navigation2DServer PROPERTY FOLDER "Plugins/Device")
endif()

### ------------------------------------------------------------------------------------

yarp_prepare_plugin(navigation2D_nws_yarp
  CATEGORY device
  TYPE navigation2D_nws_yarp
  INCLUDE navigation2D_nws_yarp.h
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(NOT SKIP_navigation2D_nws_yarp)
  yarp_add_plugin(yarp_navigation2D_nws_yarp)

  target_sources(yarp_navigation2D_nws_yarp
    PRIVATE
      navigation2D_nws_yarp.h
      navigation2D_nws_yarp.cpp
      INavigation2DServerImpl.h
      INavigation2DServerImpl.cpp
  )

  target_sources(yarp_navigation2D_nws_yarp PRIVATE $<TARGET_OBJECTS:INavigation2DMsgs>)
  target_include_directories(yarp_navigation2D_nws_yarp PRIVATE $<TARGET_PROPERTY:INavigation2DMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_navigation2D_nws_yarp
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      YARP::YARP_math
      YARP::YARP_rosmsg
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
    YARP_math
    YARP_rosmsg
  )

  yarp_install(
    TARGETS yarp_navigation2D_nws_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_navigation2D_nws_yarp PROPERTY FOLDER "Plugins/Device/NWS")
endif()
