# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(ffmpeg_grabber
                    CATEGORY device
                    TYPE FfmpegGrabber
                    INCLUDE FfmpegGrabber.h
                    EXTRA_CONFIG WRAPPER=grabberDual
                    DEPENDS "YARP_HAS_FFMPEG;FFMPEG_avformat_FOUND;FFMPEG_avdevice_FOUND")
yarp_prepare_plugin(ffmpeg_writer
                    CATEGORY device
                    TYPE FfmpegWriter
                    INCLUDE FfmpegWriter.h
                    DEPENDS "YARP_HAS_FFMPEG;FFMPEG_avformat_FOUND;FFMPEG_avdevice_FOUND")

if(NOT SKIP_ffmpeg_grabber OR NOT SKIP_ffmpeg_writer)
  yarp_add_plugin(yarp_ffmpeg)

  target_sources(yarp_ffmpeg PRIVATE FfmpegGrabber.cpp
                                     FfmpegWriter.cpp
                                     FfmpegGrabber.h
                                     FfmpegWriter.h
                                     ffmpeg_api.h
                                     ffmpeg_api.cpp)

  target_link_libraries(yarp_ffmpeg PRIVATE YARP::YARP_os
                                            YARP::YARP_sig
                                            YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  target_include_directories(yarp_ffmpeg SYSTEM PRIVATE ${FFMPEG_avcodec_INCLUDE_DIRS}
                                                        ${FFMPEG_avformat_INCLUDE_DIRS}
                                                        ${FFMPEG_avdevice_INCLUDE_DIRS})
  target_link_libraries(yarp_ffmpeg PRIVATE ${FFMPEG_avcodec_LIBRARIES}
                                            ${FFMPEG_avformat_LIBRARIES}
                                            ${FFMPEG_avdevice_LIBRARIES})
  # list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS FFMPEG) # Not using targets

  if(MINGW)
    target_link_libraries(yarp_ffmpeg PRIVATE ws2_32)
  endif()

  yarp_install(TARGETS yarp_ffmpeg
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_ffmpeg PROPERTY FOLDER "Plugins/Device")
endif()

include(YarpRemoveFile)
yarp_remove_file("${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}/ffmpeg.ini") # YARP_DEPRECATED since YARP 3.2
