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
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_HAS_FFMPEG")
yarp_prepare_plugin(ffmpeg_writer
                    CATEGORY device
                    TYPE yarp::dev::FfmpegWriter
                    INCLUDE FfmpegWriter.h
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_HAS_FFMPEG")

if(NOT SKIP_ffmpeg_grabber OR NOT SKIP_ffmpeg_writer)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  include_directories(SYSTEM ${FFMPEG_INCLUDE_DIR})
  yarp_add_plugin(yarp_ffmpeg FfmpegGrabber.cpp
                              FfmpegWriter.cpp
                              FfmpegGrabber.h
                              FfmpegWriter.h
                              ffmpeg_api.h
                              ffmpeg_api.cpp)
  target_link_libraries(yarp_ffmpeg YARP::YARP_OS
                                    YARP::YARP_sig
                                    YARP::YARP_dev
                                    ${FFMPEG_LIBRARIES})
  if(MINGW)
    target_link_libraries(yarp_ffmpeg ws2_32)
  endif()
  yarp_install(TARGETS yarp_ffmpeg
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES ffmpeg.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_ffmpeg PROPERTY FOLDER "Plugins/Device")
endif()
