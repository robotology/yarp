# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(navigation2DClient
  CATEGORY device
  TYPE Navigation2DClient
  INCLUDE Navigation2DClient.h
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(NOT SKIP_navigation2DClient)
  yarp_add_plugin(yarp_navigation2DClient)

  target_sources(yarp_navigation2DClient
    PRIVATE
      Navigation2DClient_iMap2D.cpp
      Navigation2DClient_iLocalization2D.cpp
      Navigation2DClient_iVelActs.cpp
      Navigation2DClient_iNav2DCtrl.cpp
      Navigation2DClient_iNav2DTarget.cpp
      Navigation2DClient.cpp
      Navigation2DClient.h
  )

  target_link_libraries(yarp_navigation2DClient
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      YARP::YARP_math
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
    YARP_math
  )

  yarp_install(
    TARGETS yarp_navigation2DClient
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_navigation2DClient PROPERTY FOLDER "Plugins/Device")
endif()

#----------------

yarp_prepare_plugin(navigation2D_nwc_yarp
  CATEGORY device
  TYPE Navigation2D_nwc_yarp
  INCLUDE Navigation2D_nwc_yarp.h
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(NOT SKIP_navigation2D_nwc_yarp)
  yarp_add_plugin(yarp_navigation2D_nwc_yarp)

  target_sources(yarp_navigation2D_nwc_yarp
    PRIVATE
      Navigation2D_nwc_yarp_iMap2D.cpp
      Navigation2D_nwc_yarp_iLocalization2D.cpp
      Navigation2D_nwc_yarp_iVelActs.cpp
      Navigation2D_nwc_yarp_iNav2DCtrl.cpp
      Navigation2D_nwc_yarp_iNav2DTarget.cpp
      Navigation2D_nwc_yarp.cpp
      Navigation2D_nwc_yarp.h
  )

  target_link_libraries(yarp_navigation2D_nwc_yarp
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      YARP::YARP_math
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
    YARP_math
  )

  target_sources(yarp_navigation2D_nwc_yarp PRIVATE $<TARGET_OBJECTS:IMap2DMsgs>)
  target_include_directories(yarp_navigation2D_nwc_yarp PRIVATE $<TARGET_PROPERTY:IMap2DMsgs,INTERFACE_INCLUDE_DIRECTORIES>)
  target_sources(yarp_navigation2D_nwc_yarp PRIVATE $<TARGET_OBJECTS:ILocalization2DMsgs>)
  target_include_directories(yarp_navigation2D_nwc_yarp PRIVATE $<TARGET_PROPERTY:ILocalization2DMsgs,INTERFACE_INCLUDE_DIRECTORIES>)
  target_sources(yarp_navigation2D_nwc_yarp PRIVATE $<TARGET_OBJECTS:INavigation2DMsgs>)
  target_include_directories(yarp_navigation2D_nwc_yarp PRIVATE $<TARGET_PROPERTY:INavigation2DMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  yarp_install(
    TARGETS yarp_navigation2D_nwc_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_navigation2D_nwc_yarp PROPERTY FOLDER "Plugins/Device/NWC")
endif()
