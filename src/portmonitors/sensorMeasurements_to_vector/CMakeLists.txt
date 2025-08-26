# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(sensorMeasurements_to_vector
  TYPE SensorMeasurements_to_vector
  INCLUDE SM_to_vec.h
  CATEGORY portmonitor
  DEPENDS "ENABLE_yarpcar_portmonitor"
  DEFAULT ON
)

if(SKIP_sensorMeasurements_to_vector)
  return()
endif()

yarp_add_plugin(yarp_pm_sensorMeasurements_to_vector)

target_sources(yarp_pm_sensorMeasurements_to_vector
  PRIVATE
    SM_to_vec.cpp
    SM_to_vec.h
)

target_link_libraries(yarp_pm_sensorMeasurements_to_vector
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
  TARGETS yarp_pm_sensorMeasurements_to_vector
  EXPORT YARP_${YARP_PLUGIN_MASTER}
  COMPONENT ${YARP_PLUGIN_MASTER}
  LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
  YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
)

set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

set_property(TARGET yarp_pm_sensorMeasurements_to_vector PROPERTY FOLDER "Plugins/Port Monitor")
