# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause


yarp_prepare_plugin(IMURosPublisher
  CATEGORY device
  TYPE IMURosPublisher
  INCLUDE IMURosPublisher.h
  EXTRA_CONFIG
    WRAPPER=IMURosPublisher
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(ENABLE_IMURosPublisher)
  yarp_add_plugin(yarp_IMURosPublisher)

  target_sources(yarp_IMURosPublisher
    PRIVATE
      IMURosPublisher.cpp
      IMURosPublisher.h
      GenericSensorRosPublisher.h
  )

  target_link_libraries(yarp_IMURosPublisher
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

  yarp_install(TARGETS yarp_IMURosPublisher
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_IMURosPublisher PROPERTY FOLDER "Plugins/Device")
endif()



yarp_prepare_plugin(WrenchStampedRosPublisher
  CATEGORY device
  TYPE WrenchStampedRosPublisher
  INCLUDE WrenchStampedRosPublisher.h
  EXTRA_CONFIG
    WRAPPER=WrenchStampedRosPublisher
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(ENABLE_WrenchStampedRosPublisher)
  yarp_add_plugin(yarp_WrenchStampedRosPublisher)

  target_sources(yarp_WrenchStampedRosPublisher
    PRIVATE
      WrenchStampedRosPublisher.cpp
      WrenchStampedRosPublisher.h
      GenericSensorRosPublisher.h
  )

  target_link_libraries(yarp_WrenchStampedRosPublisher
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      YARP::YARP_rosmsg
  )

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
    YARP_rosmsg
  )

  yarp_install(
    TARGETS yarp_WrenchStampedRosPublisher
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_WrenchStampedRosPublisher PROPERTY FOLDER "Plugins/Device")
endif()



yarp_prepare_plugin(TemperatureRosPublisher
  CATEGORY device
  TYPE TemperatureRosPublisher
  INCLUDE TemperatureRosPublisher.h
  EXTRA_CONFIG
    WRAPPER=TemperatureRosPublisher
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(ENABLE_TemperatureRosPublisher)
  yarp_add_plugin(yarp_TemperatureRosPublisher)

  target_sources(yarp_TemperatureRosPublisher
    PRIVATE
      TemperatureRosPublisher.cpp
      TemperatureRosPublisher.h
      GenericSensorRosPublisher.h
  )

  target_link_libraries(yarp_TemperatureRosPublisher
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      YARP::YARP_rosmsg
  )

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
    YARP_rosmsg
  )

  yarp_install(
    TARGETS yarp_TemperatureRosPublisher
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_TemperatureRosPublisher PROPERTY FOLDER "Plugins/Device")
endif()



yarp_prepare_plugin(PoseStampedRosPublisher
  CATEGORY device
  TYPE PoseStampedRosPublisher
  INCLUDE PoseStampedRosPublisher.h
  EXTRA_CONFIG
    WRAPPER=PoseStampedRosPublisher
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(ENABLE_PoseStampedRosPublisher)
  yarp_add_plugin(yarp_PoseStampedRosPublisher)

  target_sources(yarp_PoseStampedRosPublisher
    PRIVATE
      PoseStampedRosPublisher.cpp
      PoseStampedRosPublisher.h
      GenericSensorRosPublisher.h
  )

  target_link_libraries(yarp_PoseStampedRosPublisher
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
    TARGETS yarp_PoseStampedRosPublisher
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_PoseStampedRosPublisher PROPERTY FOLDER "Plugins/Device")
endif()



yarp_prepare_plugin(MagneticFieldRosPublisher
  CATEGORY device
  TYPE MagneticFieldRosPublisher
  INCLUDE MagneticFieldRosPublisher.h
  EXTRA_CONFIG
    WRAPPER=MagneticFieldRosPublisher
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(ENABLE_MagneticFieldRosPublisher)
  yarp_add_plugin(yarp_MagneticFieldRosPublisher)

  target_sources(yarp_MagneticFieldRosPublisher
    PRIVATE
      MagneticFieldRosPublisher.cpp
      MagneticFieldRosPublisher.h
      GenericSensorRosPublisher.h
    )

  target_link_libraries(yarp_MagneticFieldRosPublisher
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      YARP::YARP_rosmsg
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
    YARP_rosmsg
  )

  yarp_install(
    TARGETS yarp_MagneticFieldRosPublisher
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_MagneticFieldRosPublisher PROPERTY FOLDER "Plugins/Device")
endif()
