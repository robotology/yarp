# Copyright (C) 2010 RobotCub Consortium
# Authors: Valentina Gaggero
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(h264
                    CATEGORY carrier
                    TYPE yarp::os::H264Carrier
                    INCLUDE H264Carrier.h
                    EXTRA_CONFIG CODE=""
                    DEPENDS "CREATE_OPTIONAL_CARRIERS;GLIB2_FOUND;GSTREAMER_FOUND;GSTREAMER_APP_LIBRARY_FOUND")

if(NOT SKIP_h264)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  # GLIB2 is required by GStreamer
  include_directories(SYSTEM ${GLIB2_INCLUDE_DIR})
  include_directories(SYSTEM ${GSTREAMER_INCLUDE_DIRS})
  include_directories(SYSTEM ${GSTREAMER_app_INCLUDE_DIR})

  yarp_add_plugin(yarp_h264
                  H264Carrier.h
                  H264Carrier.cpp
                  H264Stream.h
                  H264Stream.cpp
                  H264Decoder.cpp
                  H264Decoder.h)

  target_link_libraries(yarp_h264 YARP::YARP_OS
                                  YARP::YARP_sig
                                  YARP::YARP_wire_rep_utils
                                  ${GLIB2_LIBRARIES}
                                  ${GSTREAMER_LIBRARY}
                                  ${GSTREAMER_APP_LIBRARY})

  yarp_install(TARGETS yarp_h264
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES h264.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_h264 PROPERTY FOLDER "Plugins/Carrier")
endif()
