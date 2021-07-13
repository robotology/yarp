# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(image_compression_ffmpeg
  TYPE FfmpegMonitorObject
  INCLUDE ffmpegPortmonitor.h
  CATEGORY portmonitor
  DEPENDS "ENABLE_yarpcar_portmonitor;YARP_HAS_FFMPEG"
)

if(SKIP_image_compression_ffmpeg)
  return()
endif()

yarp_add_plugin(yarp_pm_image_compression_ffmpeg)

target_sources(yarp_pm_image_compression_ffmpeg
  PRIVATE
    ffmpegPortmonitor.cpp
    ffmpegPortmonitor.h
)

target_link_libraries(yarp_pm_image_compression_ffmpeg
  PRIVATE
    YARP::YARP_os
    YARP::YARP_sig
)
list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
  YARP_os
  YARP_sig
)

target_include_directories(yarp_pm_image_compression_ffmpeg SYSTEM PRIVATE ${FFMPEG_avcodec_INCLUDE_DIRS})
target_link_libraries(yarp_pm_image_compression_ffmpeg PRIVATE ${FFMPEG_avcodec_LIBRARIES})
# list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS FFMPEG) # Not using targets

yarp_install(
  TARGETS yarp_pm_image_compression_ffmpeg
  EXPORT YARP_${YARP_PLUGIN_MASTER}
  COMPONENT ${YARP_PLUGIN_MASTER}
  LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
  YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
)

set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

set_property(TARGET yarp_pm_image_compression_ffmpeg PROPERTY FOLDER "Plugins/Port Monitor")
