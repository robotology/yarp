# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(mp3Sound TYPE Mp3SoundConverter
                                INCLUDE Mp3Sound.h
                                CATEGORY portmonitor
                                DEPENDS "ENABLE_yarpcar_portmonitor;YARP_HAS_FFMPEG")

if(NOT SKIP_mp3Sound)
  yarp_add_plugin(yarp_pm_mp3Sound)

  target_sources(yarp_pm_mp3Sound PRIVATE Mp3Sound.cpp
                                          Mp3Sound.h)
  target_link_libraries(yarp_pm_mp3Sound PRIVATE YARP::YARP_os
                                                 YARP::YARP_sig)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig)

  target_include_directories(yarp_pm_mp3Sound SYSTEM PRIVATE ${FFMPEG_INCLUDE_DIR})
  target_link_libraries(yarp_pm_mp3Sound PRIVATE ${FFMPEG_LIBRARIES})

  yarp_install(TARGETS yarp_pm_mp3Sound
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_pm_mp3Sound PROPERTY FOLDER "Plugins/Port Monitor")
endif()
