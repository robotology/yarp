# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause


yarp_prepare_plugin(frameTransformSet_nws_yarp
  CATEGORY device
  TYPE FrameTransformSet_nws_yarp
  INCLUDE FrameTransformSet_nws_yarp.h
  DEPENDS "TARGET YARP::YARP_math"
  EXTRA_CONFIG
    WRAPPER=frameTransformSet_nws_yarp
  DEFAULT ON
)

if(NOT SKIP_frameTransformSet_nws_yarp)
  yarp_add_plugin(yarp_frameTransformSet_nws_yarp)

  target_sources(yarp_frameTransformSet_nws_yarp
    PRIVATE
      FrameTransformSet_nws_yarp.cpp
      FrameTransformSet_nws_yarp.h
  )

  target_sources(yarp_frameTransformSet_nws_yarp PRIVATE $<TARGET_OBJECTS:frameTransformStorageMsgs>)
  target_include_directories(yarp_frameTransformSet_nws_yarp PRIVATE $<TARGET_PROPERTY:frameTransformStorageMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_frameTransformSet_nws_yarp
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
  )

  yarp_install(
    TARGETS yarp_frameTransformSet_nws_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_frameTransformSet_nws_yarp PROPERTY FOLDER "Plugins/Device/NWS")
endif()



yarp_prepare_plugin(frameTransformSet_nwc_yarp
  CATEGORY device
  TYPE FrameTransformSet_nwc_yarp
  INCLUDE FrameTransformSet_nwc_yarp.h
  DEPENDS "TARGET YARP::YARP_math"
  EXTRA_CONFIG
    WRAPPER=frameTransformSet_nws_yarp
  DEFAULT ON
)

if(NOT SKIP_frameTransformSet_nwc_yarp)
  yarp_add_plugin(yarp_frameTransformSet_nwc_yarp)

  target_sources(yarp_frameTransformSet_nwc_yarp
    PRIVATE
      FrameTransformSet_nwc_yarp.cpp
      FrameTransformSet_nwc_yarp.h
  )

  target_sources(yarp_frameTransformSet_nwc_yarp PRIVATE $<TARGET_OBJECTS:frameTransformStorageMsgs>)
  target_include_directories(yarp_frameTransformSet_nwc_yarp PRIVATE $<TARGET_PROPERTY:frameTransformStorageMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_frameTransformSet_nwc_yarp
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
  )

  yarp_install(
    TARGETS yarp_frameTransformSet_nwc_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_frameTransformSet_nwc_yarp PROPERTY FOLDER "Plugins/Device/NWC")
endif()



yarp_prepare_plugin(frameTransformSet_nwc_ros
  CATEGORY device
  TYPE FrameTransformSet_nwc_ros
  INCLUDE FrameTransformSet_nwc_ros.h
  DEPENDS "TARGET YARP::YARP_math"
  EXTRA_CONFIG
    WRAPPER=frameTransformSet_nws_yarp
  DEFAULT ON
)

if(NOT SKIP_frameTransformSet_nwc_ros)
  yarp_add_plugin(yarp_frameTransformSet_nwc_ros)

  target_sources(yarp_frameTransformSet_nwc_ros
    PRIVATE
      FrameTransformSet_nwc_ros.cpp
      FrameTransformSet_nwc_ros.h
  )

  target_sources(yarp_frameTransformSet_nwc_ros PRIVATE $<TARGET_OBJECTS:FrameTransformUtils>)
  target_include_directories(yarp_frameTransformSet_nwc_ros PRIVATE $<TARGET_PROPERTY:FrameTransformUtils,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_frameTransformSet_nwc_ros
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
    TARGETS yarp_frameTransformSet_nwc_ros
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_frameTransformSet_nwc_ros PROPERTY FOLDER "Plugins/Device/NWC")
endif()



yarp_prepare_plugin(frameTransformSetMultiplexer
  CATEGORY device
  TYPE FrameTransformSetMultiplexer
  INCLUDE FrameTransformSetMultiplexer.h
  DEPENDS "TARGET YARP::YARP_math"
  EXTRA_CONFIG
    WRAPPER=frameTransformSet_nws_yarp
  DEFAULT ON
)

if(NOT SKIP_frameTransformSetMultiplexer)
  yarp_add_plugin(yarp_frameTransformSetMultiplexer)

  target_sources(yarp_frameTransformSetMultiplexer
    PRIVATE
      FrameTransformSetMultiplexer.cpp
      FrameTransformSetMultiplexer.h
  )

  target_link_libraries(yarp_frameTransformSetMultiplexer
    PRIVATE
      YARP::YARP_os
      YARP::YARP_dev
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_dev
  )

  yarp_install(
    TARGETS yarp_frameTransformSetMultiplexer
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_frameTransformSetMultiplexer PROPERTY FOLDER "Plugins/Device")
endif()
