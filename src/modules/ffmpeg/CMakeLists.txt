# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
IF (COMPILE_DEVICE_LIBRARY)
  PREPARE_DEVICE(ffmpeg_grabber TYPE FfmpegGrabber INCLUDE FfmpegGrabber.h WRAPPER grabber)
  PREPARE_DEVICE(ffmpeg_writer TYPE FfmpegWriter INCLUDE FfmpegWriter.h)
  ADD_DEVICE_FINGERPRINT(ffmpeg.ini ffmpeg_grabber ffmpeg_writer)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (SKIP_ffmpeg_grabber AND SKIP_ffmpeg_writer)
  SET(SKIP_ffmpeg TRUE)
ENDIF (SKIP_ffmpeg_grabber AND SKIP_ffmpeg_writer)


IF (NOT SKIP_ffmpeg)

  FIND_PACKAGE(YARP REQUIRED)

  FIND_PACKAGE(FFMPEG REQUIRED)

  IF (FFMPEG_FOUND)

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${FFMPEG_INCLUDE_DIR})
    INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS} ${ACE_INCLUDE_DIRS})
    LINK_LIBRARIES(${FFMPEG_LIBRARIES} ${YARP_LIBRARIES})
    IF (MINGW)
      LINK_LIBRARIES(ws2_32)
    ENDIF (MINGW)

    ADD_LIBRARY(yarp_ffmpeg FfmpegGrabber.cpp FfmpegWriter.cpp FfmpegGrabber.h FfmpegWriter.h ffmpeg_api.h ffmpeg_api.cpp)

  ELSE (FFMPEG_FOUND)

    MESSAGE(SEND_ERROR "ffmpeg not found")

  ENDIF (FFMPEG_FOUND)


ENDIF (NOT SKIP_ffmpeg)
