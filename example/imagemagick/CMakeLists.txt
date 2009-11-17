# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

PROJECT(hello)

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" )

# replace "..." with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)
FIND_PACKAGE(ImageMagickLib)

IF(IMAGEMAGICKLIB_FOUND)
    MESSAGE(STATUS "ImageMagickLib found")
    INCLUDE_DIRECTORIES(${IMAGEMAGICKLIB_INCLUDE_DIR})
    LINK_DIRECTORIES(${IMAGEMAGICKLIB_LINK_DIRECTORIES})
    LINK_LIBRARIES(${IMAGEMAGICKLIB_LIBRARIES})
ELSE(IMAGEMAGICKLIB_FOUND)
    MESSAGE(ERROR "ImageMagickLib requested but not found.")
ENDIF(IMAGEMAGICKLIB_FOUND)

ADD_EXECUTABLE(magicktest main.cpp)
