# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(mobileBaseVelocityControl_nwc_yarp
  CATEGORY device
  TYPE MobileBaseVelocityControl_nwc_yarp
  INCLUDE MobileBaseVelocityControl_nwc_yarp.h
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(NOT SKIP_mobileBaseVelocityControl_nwc_yarp)
  yarp_add_plugin(yarp_mobileBaseVelocityControl_nwc_yarp)

  target_sources(yarp_mobileBaseVelocityControl_nwc_yarp
    PRIVATE
      MobileBaseVelocityControl_nwc_yarp.cpp
      MobileBaseVelocityControl_nwc_yarp.h
  )

  target_sources(yarp_mobileBaseVelocityControl_nwc_yarp PRIVATE $<TARGET_OBJECTS:MobileBaseVelocityControlMsgs>)
  target_include_directories(yarp_mobileBaseVelocityControl_nwc_yarp PRIVATE $<TARGET_PROPERTY:MobileBaseVelocityControlMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_mobileBaseVelocityControl_nwc_yarp
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
    TARGETS yarp_mobileBaseVelocityControl_nwc_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_mobileBaseVelocityControl_nwc_yarp PROPERTY FOLDER "Plugins/Device/NWC")
endif()






yarp_prepare_plugin(mobileBaseVelocityControl_nws_yarp
  CATEGORY device
  TYPE MobileBaseVelocityControl_nws_yarp
  INCLUDE MobileBaseVelocityControl_nws_yarp.h
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(NOT SKIP_mobileBaseVelocityControl_nws_yarp)
  yarp_add_plugin(yarp_mobileBaseVelocityControl_nws_yarp)

  target_sources(yarp_mobileBaseVelocityControl_nws_yarp
    PRIVATE
      MobileBaseVelocityControl_nws_yarp.cpp
      MobileBaseVelocityControl_nws_yarp.h
  )

  target_sources(yarp_mobileBaseVelocityControl_nws_yarp PRIVATE $<TARGET_OBJECTS:MobileBaseVelocityControlMsgs>)
  target_include_directories(yarp_mobileBaseVelocityControl_nws_yarp PRIVATE $<TARGET_PROPERTY:MobileBaseVelocityControlMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_mobileBaseVelocityControl_nws_yarp
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
    TARGETS yarp_mobileBaseVelocityControl_nws_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_mobileBaseVelocityControl_nws_yarp PROPERTY FOLDER "Plugins/Device/NWS")
endif()







yarp_prepare_plugin(mobileBaseVelocityControl_nwc_ros
  CATEGORY device
  TYPE MobileBaseVelocityControl_nws_ros
  INCLUDE MobileBaseVelocityControl_nws_ros.h
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(NOT SKIP_mobileBaseVelocityControl_nwc_ros)
  yarp_add_plugin(yarp_mobileBaseVelocityControl_nws_ros)

  target_sources(yarp_mobileBaseVelocityControl_nws_ros
    PRIVATE
      MobileBaseVelocityControl_nws_ros.cpp
      MobileBaseVelocityControl_nws_ros.h
  )

  target_link_libraries(yarp_mobileBaseVelocityControl_nws_ros
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
    TARGETS yarp_mobileBaseVelocityControl_nws_ros
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_mobileBaseVelocityControl_nws_ros PROPERTY FOLDER "Plugins/Device/NWS")
endif()
