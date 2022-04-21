# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(mobileBaseVelocityControl_nws_ros
  CATEGORY device
  TYPE MobileBaseVelocityControl_nws_ros
  INCLUDE MobileBaseVelocityControl_nws_ros.h
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(NOT SKIP_mobileBaseVelocityControl_nws_ros)
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
