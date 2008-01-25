IF (COMPILE_DEVICE_LIBRARY)
  PREPARE_DEVICE(ffmpeg_grabber TYPE FfmpegGrabber INCLUDE common/yarp/FfmpegGrabber.h WRAPPER grabber)
  PREPARE_DEVICE(ffmpeg_writer TYPE FfmpegWriter INCLUDE common/yarp/FfmpegWriter.h)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (SKIP_ffmpeg_grabber AND SKIP_ffmpeg_writer)
  SET(SKIP_ffmpeg TRUE)
ENDIF (SKIP_ffmpeg_grabber AND SKIP_ffmpeg_writer)


IF (NOT SKIP_ffmpeg)

FIND_PACKAGE(YARP REQUIRED)

FIND_PACKAGE(FFMPEG)

IF (FFMPEG_FOUND)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common ${FFMPEG_INCLUDE_DIR})
  LINK_LIBRARIES(${FFMPEG_LIBRARIES})
  IF (MINGW)
    LINK_LIBRARIES(ws2_32)
  ENDIF (MINGW)

  ADD_LIBRARY(ffmpeg default/yarp/FfmpegGrabber.cpp default/yarp/FfmpegWriter.cpp common/yarp/FfmpegGrabber.h common/yarp/FfmpegWriter.h)

ELSE (FFMPEG_FOUND)

  MESSAGE(STATUS "ffmpeg not found")

ENDIF (FFMPEG_FOUND)


ENDIF (NOT SKIP_ffmpeg)
