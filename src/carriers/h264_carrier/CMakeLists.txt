# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(h264
  CATEGORY carrier
  TYPE H264Carrier
  INCLUDE H264Carrier.h
  DEPENDS "YARP_HAS_GObject;YARP_HAS_GLIB2;YARP_HAS_GStreamer;YARP_HAS_GStreamerPluginsBase"
)

if(NOT SKIP_h264)
  yarp_add_plugin(yarp_h264)

  target_sources(yarp_h264
    PRIVATE
      H264Carrier.h
      H264Carrier.cpp
      H264Stream.h
      H264Stream.cpp
      H264Decoder.cpp
      H264Decoder.h
      H264LogComponent.cpp
      H264LogComponent.h
  )

  target_link_libraries(yarp_h264
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_wire_rep_utils
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
  )

  # GObject is required by GStreamer
  target_link_libraries(yarp_h264 PRIVATE ${GOBJECT_LIBRARIES})
  target_include_directories(yarp_h264 SYSTEM PRIVATE ${GOBJECT_INCLUDE_DIR})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS GObject) (not using targets)

  # GLIB2 is required by GStreamer
  target_link_libraries(yarp_h264 PRIVATE ${GLIB2_LIBRARIES})
  target_include_directories(yarp_h264 SYSTEM PRIVATE ${GLIB2_INCLUDE_DIR})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS GLIB2) (not using targets)

  target_include_directories(yarp_h264 SYSTEM PRIVATE ${GSTREAMER_INCLUDE_DIRS})
  target_link_libraries(yarp_h264 PRIVATE ${GSTREAMER_LIBRARY})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS GSTREAMER) (not using targets)

  target_include_directories(yarp_h264 SYSTEM PRIVATE ${GSTREAMER_app_INCLUDE_DIR})
  target_link_libraries(yarp_h264 PRIVATE ${GSTREAMER_APP_LIBRARY})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS GStreamerPluginsBase) (not using targets)

  yarp_install(
    TARGETS yarp_h264
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_h264 PROPERTY FOLDER "Plugins/Carrier")
endif()
