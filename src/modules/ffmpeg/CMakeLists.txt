# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(ffmpeg_grabber TYPE yarp::dev::FfmpegGrabber INCLUDE FfmpegGrabber.h WRAPPER grabber)
  yarp_prepare_device(ffmpeg_writer TYPE yarp::dev::FfmpegWriter INCLUDE FfmpegWriter.h)
  yarp_install(FILES ffmpeg.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()

if(SKIP_ffmpeg_grabber AND SKIP_ffmpeg_writer)
  set(SKIP_ffmpeg TRUE)
endif()

if(NOT SKIP_ffmpeg)
  find_package(YARP REQUIRED)
  find_package(FFMPEG REQUIRED)
  if(FFMPEG_FOUND)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                        ${YARP_INCLUDE_DIRS})
    include_directories(SYSTEM ${FFMPEG_INCLUDE_DIR})
    yarp_add_plugin(yarp_ffmpeg FfmpegGrabber.cpp FfmpegWriter.cpp FfmpegGrabber.h FfmpegWriter.h ffmpeg_api.h ffmpeg_api.cpp)
    target_link_libraries(yarp_ffmpeg ${YARP_LIBRARIES} ${FFMPEG_LIBRARIES})
    if(MINGW)
      target_link_libraries(yarp_ffmpeg ws2_32)
    endif()
    yarp_install(TARGETS yarp_ffmpeg
                 EXPORT YARP
                 COMPONENT runtime
                 LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  else()
    message(SEND_ERROR "ffmpeg not found")
  endif()
endif()
