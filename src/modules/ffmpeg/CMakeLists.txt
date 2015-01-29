# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(ffmpeg_grabber TYPE FfmpegGrabber INCLUDE FfmpegGrabber.h WRAPPER grabber)
  yarp_prepare_device(ffmpeg_writer TYPE FfmpegWriter INCLUDE FfmpegWriter.h)
  yarp_add_device_fingerprint(ffmpeg.ini ffmpeg_grabber ffmpeg_writer)
endif()

if(SKIP_ffmpeg_grabber AND SKIP_ffmpeg_writer)
  set(SKIP_ffmpeg TRUE)
endif()

if(NOT SKIP_ffmpeg)
  find_package(YARP REQUIRED)
  find_package(FFMPEG REQUIRED)
  if(FFMPEG_FOUND)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${FFMPEG_INCLUDE_DIR})
    include_directories(${YARP_INCLUDE_DIRS} ${ACE_INCLUDE_DIRS})
    yarp_add_plugin(yarp_ffmpeg FfmpegGrabber.cpp FfmpegWriter.cpp FfmpegGrabber.h FfmpegWriter.h ffmpeg_api.h ffmpeg_api.cpp)
    target_link_libraries(yarp_ffmpeg ${YARP_LIBRARIES} ${FFMPEG_LIBRARIES})
    if(MINGW)
      target_link_libraries(yarp_ffmpeg ws2_32)
    endif()

  else()
    message(SEND_ERROR "ffmpeg not found")
  endif()
endif()
