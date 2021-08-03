# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause


yarp_prepare_plugin(soundfilter_resample
  TYPE SoundFilter_resample
  INCLUDE SoundFilter_resample.h
  CATEGORY portmonitor
  DEPENDS "ENABLE_yarpcar_portmonitor"
)

if(SKIP_soundfilter_resample)
  return()
endif()

yarp_add_plugin(yarp_pm_soundfilter_resample)

target_sources(yarp_pm_soundfilter_resample
  PRIVATE
    SoundFilter_resample.cpp
    SoundFilter_resample.h
)

target_link_libraries(yarp_pm_soundfilter_resample
  PRIVATE
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_dev
)

yarp_install(
  TARGETS yarp_pm_soundfilter_resample
  EXPORT yarp_pm_soundfilter_resample
  COMPONENT runtime
  LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
  YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
)
set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

set_property(TARGET yarp_pm_soundfilter_resample PROPERTY FOLDER "Plugins/Port Monitor")
