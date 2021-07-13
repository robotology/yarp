# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(depthimage_compression_zfp
  TYPE ZfpMonitorObject
  INCLUDE zfpPortmonitor.h
  CATEGORY portmonitor
  DEPENDS "ENABLE_yarpcar_portmonitor;YARP_HAS_ZFP"
)

yarp_renamed_option(ENABLE_yarpcar_zfp ENABLE_yarppm_depthimage_compression_zfp) # YARP_DEPRECATED since 3.5
yarp_remove_file("${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}/zfp.ini") # YARP_DEPRECATED since 3.5

if(SKIP_depthimage_compression_zfp)
  return()
endif()

yarp_add_plugin(yarp_pm_depthimage_compression_zfp)

target_sources(yarp_pm_depthimage_compression_zfp
  PRIVATE
    zfpPortmonitor.cpp
    zfpPortmonitor.h
)

target_link_libraries(yarp_pm_depthimage_compression_zfp
  PRIVATE
    YARP::YARP_os
    YARP::YARP_sig
)
list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
  YARP_os
  YARP_sig
)

target_include_directories(yarp_pm_depthimage_compression_zfp SYSTEM PRIVATE ${ZFP_INCLUDE_DIRS})
target_link_libraries(yarp_pm_depthimage_compression_zfp PRIVATE ${ZFP_LIBRARIES})
# list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ZFP) (not using targets)

yarp_install(
  TARGETS yarp_pm_depthimage_compression_zfp
  EXPORT YARP_${YARP_PLUGIN_MASTER}
  COMPONENT ${YARP_PLUGIN_MASTER}
  LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
  YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
)

set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

set_property(TARGET yarp_pm_depthimage_compression_zfp PROPERTY FOLDER "Plugins/Port Monitor")
