# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_carrier(mjpeg_carrier TYPE MjpegCarrier INCLUDE MjpegCarrier.h)
  yarp_add_carrier_fingerprint(mjpeg.ini mjpeg_carrier)
endif (COMPILE_PLUGIN_LIBRARY)

if (NOT SKIP_mjpeg_carrier)
  option(MJPEG_AUTOCOMPRESS "Automatically compress/decompress images on mjpeg stream" TRUE)
  if (MJPEG_AUTOCOMPRESS) 
    add_definitions(-DMJPEG_AUTOCOMPRESS)
  endif ()
  find_package(YARP REQUIRED)
  find_package(JPEG REQUIRED)
  # on windows, a handy place to get libjpeg in compiled form is:
  #   svn checkout https://svn.blender.org/svnroot/bf-blender/trunk/lib/windows/jpeg (for 32-bit)
  #   svn checkout https://svn.blender.org/svnroot/bf-blender/trunk/lib/win64/jpeg (for 64-bit)
  include_directories(${JPEG_INCLUDE_DIR})
  include_directories(${YARP_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  get_property(WIRE_INCLUDE_DIRS TARGET YARP_wire_rep_utils PROPERTY INCLUDE_DIRS)
  include_directories(${WIRE_INCLUDE_DIRS})
  yarp_add_plugin(mjpeg_carrier MjpegCarrier.h MjpegCarrier.cpp
                  MjpegStream.h MjpegStream.cpp
                  MjpegDecompression.h MjpegDecompression.cpp
                  ${JPEG_SOURCES})
  target_link_libraries(mjpeg_carrier ${YARP_LIBRARIES} ${JPEG_LIBRARY})
  target_link_libraries(mjpeg_carrier YARP_wire_rep_utils)


if(YARP_COMPILE_TESTS)
  add_executable(test_mjpeg test_mjpeg.cpp)
  target_link_libraries(test_mjpeg mjpeg_carrier YARP_init)
endif(YARP_COMPILE_TESTS)

endif (NOT SKIP_mjpeg_carrier)
