# Copyright (C) 2017  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

include_directories("${CMAKE_SOURCE_DIR}/src/carriers/mjpeg_carrier/")

add_executable(test_mjpeg test_mjpeg.cpp
                          ${CMAKE_SOURCE_DIR}/src/carriers/mjpeg_carrier/MjpegDecompression.h
                          ${CMAKE_SOURCE_DIR}/src/carriers/mjpeg_carrier/MjpegDecompression.cpp)
target_link_libraries(test_mjpeg YARP_OS
                                 YARP_sig
                                 YARP_init)
target_link_libraries(test_mjpeg ${JPEG_LIBRARY})
set_property(TARGET test_mjpeg PROPERTY FOLDER "Test")
