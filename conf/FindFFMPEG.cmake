# from VTK project

#
# Find the native FFMPEG includes and library
#
# This module defines
# FFMPEG_INCLUDE_DIR, where to find avcodec.h, avformat.h ...
# FFMPEG_LIBRARIES, the libraries to link against to use FFMPEG.
# FFMPEG_FOUND, If false, do not try to use FFMPEG.

# also defined, but not for general use are
# FFMPEG_avformat_LIBRARY and FFMPEG_avcodec_LIBRARY, where to find the FFMPEG library.
# This is usefull to do it this way so that we can always add more libraries
# if needed to FFMPEG_LIBRARIES if ffmpeg ever changes...

FIND_PATH(FFMPEG_INCLUDE_DIR ffmpeg/avformat.h
  /usr/local/include
  /usr/include
  $ENV{FFMPEG_DIR}
  $ENV{FFMPEG_DIR}/include
)

FIND_LIBRARY(FFMPEG_avformat_LIBRARY avformat
  /usr/local/lib
  /usr/lib
  $ENV{FFMPEG_DIR}
  $ENV{FFMPEG_DIR}/lib
)

FIND_LIBRARY(FFMPEG_avcodec_LIBRARY avcodec
  /usr/local/lib
  /usr/lib
  $ENV{FFMPEG_DIR}
  $ENV{FFMPEG_DIR}/lib
)

FIND_LIBRARY(FFMPEG_avutil_LIBRARY avutil
  /usr/local/lib
  /usr/lib
  $ENV{FFMPEG_DIR}
  $ENV{FFMPEG_DIR}/lib
)

FIND_LIBRARY(_FFMPEG_z_LIBRARY_ z
  /usr/local/lib
  /usr/lib
  $ENV{FFMPEG_DIR}
  $ENV{FFMPEG_DIR}/lib
)



IF(FFMPEG_INCLUDE_DIR)
  IF(FFMPEG_avformat_LIBRARY)
    IF(FFMPEG_avcodec_LIBRARY)
      IF(FFMPEG_avutil_LIBRARY)
        SET( FFMPEG_FOUND "YES" )
        SET( FFMPEG_LIBRARIES 
          ${FFMPEG_avformat_LIBRARY}
          ${FFMPEG_avcodec_LIBRARY} 
          ${FFMPEG_avutil_LIBRARY} 
          )
	IF(_FFMPEG_z_LIBRARY_)
          SET( FFMPEG_LIBRARIES 
	    ${FFMPEG_LIBRARIES}
            ${_FFMPEG_z_LIBRARY_}
          )
	ENDIF(_FFMPEG_z_LIBRARY_)
      ENDIF(FFMPEG_avutil_LIBRARY)
    ENDIF(FFMPEG_avcodec_LIBRARY)
  ENDIF(FFMPEG_avformat_LIBRARY)
ENDIF(FFMPEG_INCLUDE_DIR)

MARK_AS_ADVANCED(
  FFMPEG_INCLUDE_DIR
  FFMPEG_avformat_LIBRARY
  FFMPEG_avcodec_LIBRARY
  FFMPEG_avutil_LIBRARY
  _FFMPEG_z_LIBRARY_
  )

