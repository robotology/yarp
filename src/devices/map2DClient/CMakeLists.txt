# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(map2DClient
  CATEGORY device
  TYPE Map2DClient
  INCLUDE Map2DClient.h
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(NOT SKIP_map2DClient)
  yarp_add_plugin(yarp_map2DClient)

  target_sources(yarp_map2DClient
    PRIVATE
      Map2DClient.cpp
      Map2DClient.h
  )

  target_link_libraries(yarp_map2DClient
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
    TARGETS yarp_map2DClient
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_map2DClient PROPERTY FOLDER "Plugins/Device")
endif()





yarp_prepare_plugin(map2D_nwc_yarp
  CATEGORY device
  TYPE Map2D_nwc_yarp
  INCLUDE Map2D_nwc_yarp.h
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(NOT SKIP_map2D_nwc_yarp)
  yarp_add_plugin(yarp_map2D_nwc_yarp)

  target_sources(yarp_map2D_nwc_yarp
    PRIVATE
      Map2D_nwc_yarp.cpp
      Map2D_nwc_yarp.h
  )

  target_sources(yarp_map2D_nwc_yarp PRIVATE $<TARGET_OBJECTS:IMap2DMsgs>)
  target_include_directories(yarp_map2D_nwc_yarp PRIVATE $<TARGET_PROPERTY:IMap2DMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_map2D_nwc_yarp
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
    TARGETS yarp_map2D_nwc_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_map2D_nwc_yarp PROPERTY FOLDER "Plugins/Device/NWC")
endif()
