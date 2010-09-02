# Copyright (C) 2007 Giacomo Spigler
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# replace with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP REQUIRED)

FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIR})
LINK_DIRECTORIES(${OPENCV_LINK_DIRECTORIES})
LINK_LIBRARIES(${OPENCV_LIBRARIES})

ADD_EXECUTABLE(cuda_gpu cuda_gpu.cpp)
