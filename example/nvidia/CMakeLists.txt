# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2007 Giacomo Spigler
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

# replace with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP REQUIRED)

FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIR})
LINK_DIRECTORIES(${OPENCV_LINK_DIRECTORIES})
LINK_LIBRARIES(${OPENCV_LIBRARIES})

LINK_LIBRARIES(pthread GL glut CgGL)
IF (NOT WINDOWS)
LINK_LIBRARIES(GLEW)   
ENDIF (NOT WINDOWS)
ADD_EXECUTABLE(nvidia_gpu nvidia_gpu.cpp)
