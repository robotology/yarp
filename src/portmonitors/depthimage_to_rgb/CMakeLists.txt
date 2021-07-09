# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(depthimage_to_rgb
  TYPE DepthImageConverter
  INCLUDE DepthImage2.h
  CATEGORY portmonitor
  DEPENDS "ENABLE_yarpcar_portmonitor"
)

yarp_renamed_option(ENABLE_yarpcar_depthimage2 ENABLE_yarppm_depthimage_to_rgb) # YARP_DEPRECATED since 3.5
yarp_remove_file("${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}/depthimage2.ini") # YARP_DEPRECATED since 3.5

if(SKIP_depthimage_to_rgb)
  return()
endif()

yarp_add_plugin(yarp_pm_depthimage_to_rgb)

target_sources(yarp_pm_depthimage_to_rgb
  PRIVATE
    DepthImage2.cpp
    DepthImage2.h
)
target_link_libraries(yarp_pm_depthimage_to_rgb
  PRIVATE
    YARP::YARP_os
    YARP::YARP_sig
)
list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
  YARP_os
  YARP_sig
)

yarp_install(
  TARGETS yarp_pm_depthimage_to_rgb
  EXPORT YARP_${YARP_PLUGIN_MASTER}
  COMPONENT ${YARP_PLUGIN_MASTER}
  LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
  YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
)

set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

set_property(TARGET yarp_pm_depthimage_to_rgb PROPERTY FOLDER "Plugins/Port Monitor")
