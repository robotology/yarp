# SPDX-FileCopyrightText: 2024 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(gstreamer
  CATEGORY carrier
  TYPE GstreamerCarrier
  INCLUDE GstreamerCarrier.h
  DEPENDS "YARP_HAS_GObject;YARP_HAS_GLIB2;YARP_HAS_GStreamer;YARP_HAS_GStreamerPluginsBase"
)

if(NOT SKIP_gstreamer)
  yarp_add_plugin(yarp_gstreamer)

  if(YARP_HAS_ACE)
  target_compile_definitions(yarp_gstreamer PRIVATE YARP_HAS_ACE)
  target_link_libraries(yarp_gstreamer PRIVATE ACE::ACE)
  endif()

  target_sources(yarp_gstreamer
    PRIVATE
      GstreamerCarrier.h
      GstreamerCarrier.cpp
      GstreamerStream.h
      GstreamerStream.cpp
      GstreamerDecoder.h
      GstreamerDecoder.cpp
  )

  target_link_libraries(yarp_gstreamer
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
  )

  #Gstreamer stuff
  target_link_libraries(yarp_gstreamer PRIVATE ${GOBJECT_LIBRARIES})
  target_include_directories(yarp_gstreamer SYSTEM PRIVATE ${GOBJECT_INCLUDE_DIR})
  target_link_libraries(yarp_gstreamer PRIVATE ${GLIB2_LIBRARIES})
  target_include_directories(yarp_gstreamer SYSTEM PRIVATE ${GLIB2_INCLUDE_DIR})
  target_include_directories(yarp_gstreamer SYSTEM PRIVATE ${GSTREAMER_INCLUDE_DIRS})
  target_link_libraries(yarp_gstreamer PRIVATE ${GSTREAMER_LIBRARY})
  target_include_directories(yarp_gstreamer SYSTEM PRIVATE ${GSTREAMER_app_INCLUDE_DIR})
  target_link_libraries(yarp_gstreamer PRIVATE ${GSTREAMER_APP_LIBRARY})

  yarp_install(
    TARGETS yarp_gstreamer
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_gstreamer PROPERTY FOLDER "Plugins/Carrier")
endif()
