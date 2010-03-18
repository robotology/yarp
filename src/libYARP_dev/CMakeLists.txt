# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
 
PROJECT(libYARP_dev)

FILE(GLOB folder_source src/*.cpp)
FILE(GLOB_RECURSE folder_header include/*.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

SET(populator ${CMAKE_BINARY_DIR}/src/libYARP_dev/src_generated/PopulateDrivers.cpp)

SET_PROPERTY(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS 
  ${PROJECT_SOURCE_DIR}/include)
GET_PROPERTY(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
INCLUDE_DIRECTORIES(${YARP_TREE_INCLUDE_DIRS})

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/src/libYARP_dev/src/PopulateDrivers.cpp.in
  ${populator} @ONLY  IMMEDIATE)


ADD_LIBRARY(YARP_dev ${folder_source} ${folder_header} ${populator})
TARGET_LINK_LIBRARIES(YARP_dev YARP_sig YARP_OS)

SET_PROPERTY(TARGET YARP_dev PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})

