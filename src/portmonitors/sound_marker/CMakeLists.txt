# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause


yarp_prepare_plugin(sound_marker
  TYPE Sound_marker
  INCLUDE Sound_marker.h
  CATEGORY portmonitor
  DEPENDS "ENABLE_yarpcar_portmonitor"
)

if(SKIP_sound_marker)
  return()
endif()

yarp_add_plugin(yarp_pm_sound_marker)

target_sources(yarp_pm_sound_marker
  PRIVATE
    Sound_marker.cpp
    Sound_marker.h
)

target_link_libraries(yarp_pm_sound_marker
  PRIVATE
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_dev
)

yarp_install(
  TARGETS yarp_pm_sound_marker
  EXPORT yarp_pm_sound_marker
  COMPONENT runtime
  LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
  YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
)
set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

set_property(TARGET yarp_pm_sound_marker PROPERTY FOLDER "Plugins/Port Monitor")
