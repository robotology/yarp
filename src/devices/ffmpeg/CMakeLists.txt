# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(ffmpeg_grabber
                    CATEGORY device
                    TYPE yarp::dev::FfmpegGrabber
                    INCLUDE FfmpegGrabber.h
                    EXTRA_CONFIG WRAPPER=grabber
                    DEPENDS "YARP_HAS_FFMPEG")
yarp_prepare_plugin(ffmpeg_writer
                    CATEGORY device
                    TYPE yarp::dev::FfmpegWriter
                    INCLUDE FfmpegWriter.h
                    DEPENDS "YARP_HAS_FFMPEG")

if(NOT SKIP_ffmpeg_grabber OR NOT SKIP_ffmpeg_writer)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  yarp_add_plugin(yarp_ffmpeg FfmpegGrabber.cpp
                              FfmpegWriter.cpp
                              FfmpegGrabber.h
                              FfmpegWriter.h
                              ffmpeg_api.h
                              ffmpeg_api.cpp)
  target_link_libraries(yarp_ffmpeg PRIVATE YARP::YARP_OS
                                            YARP::YARP_sig
                                            YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_OS
                                                      YARP_sig
                                                      YARP_dev)

  target_include_directories(yarp_ffmpeg SYSTEM PRIVATE ${FFMPEG_INCLUDE_DIR})
  target_link_libraries(yarp_ffmpeg PRIVATE ${FFMPEG_LIBRARIES})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS FFMPEG) (not using targets)

  if(MINGW)
    target_link_libraries(yarp_ffmpeg PRIVATE ws2_32)
  endif()

  yarp_install(TARGETS yarp_ffmpeg
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES ffmpeg.ini
               COMPONENT ${YARP_PLUGIN_MASTER}
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_ffmpeg PROPERTY FOLDER "Plugins/Device")
endif()
