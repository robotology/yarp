# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(YARP_HAS_JPEG)
  if(MJPEG_AUTOCOMPRESS)
    add_definitions(-DMJPEG_AUTOCOMPRESS)
  endif()

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS})
  include_directories(SYSTEM ${JPEG_INCLUDE_DIR})
  include_directories("${CMAKE_SOURCE_DIR}/src/carriers/mjpeg_carrier/")

  add_executable(test_mjpeg test_mjpeg.cpp
                            ${CMAKE_SOURCE_DIR}/src/carriers/mjpeg_carrier/MjpegDecompression.h
                            ${CMAKE_SOURCE_DIR}/src/carriers/mjpeg_carrier/MjpegDecompression.cpp)
  target_link_libraries(test_mjpeg YARP_OS
                                   YARP_sig
                                   YARP_init)
  target_link_libraries(test_mjpeg ${JPEG_LIBRARY})
  set_property(TARGET test_mjpeg PROPERTY FOLDER "Test")
endif()
