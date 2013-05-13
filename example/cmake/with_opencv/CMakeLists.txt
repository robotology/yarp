# Copyright: (C) 2010 RobotCub Consortium
# Author: Lorenzo Natale, Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

  cmake_minimum_required(VERSION 2.8.7)

  # execute YarpConfig.cmake
  FIND_PACKAGE(YARP)
  
  # pick up YARP find scripts
  SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${YARP_MODULE_PATH})
  
  # now FindOpenCV.cmake is in CMAKE_MODULE_PATH
  FIND_PACKAGE(OpenCV)
  
  # searches for all .cpp and .h files in the current directory and add them 
  # to the current project
  FILE(GLOB folder_source *.cpp)
  FILE(GLOB folder_header *.h)
  SOURCE_GROUP("Source Files" FILES ${folder_source})
  SOURCE_GROUP("Header Files" FILES ${folder_header})
  
  # add the current directory in the include path
  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
  # add YARP include directories
  INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
  # add OpenCV include directories
  INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIR})
   
  # create the program "foo"
  ADD_EXECUTABLE(foo ${folder_source} ${folder_header})
   
  # link opencv and YARP libraries
  TARGET_LINK_LIBRARIES(foo ${OPENCV_LIBRARIES} ${YARP_LIBRARIES})
