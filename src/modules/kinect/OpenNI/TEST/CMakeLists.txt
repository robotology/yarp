# Copyright (C) 2011 Duarte Aragao
# Author: Duarte Aragao
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
cmake_minimum_required(VERSION 2.6.4)

  FIND_PACKAGE(YARP REQUIRED)
  LINK_LIBRARIES(${YARP_LIBRARIES})
    INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

FIND_PACKAGE(GLUT)
FIND_PACKAGE(OpenGL)

 #OPENNI Lib MESSAGE("$ENV{OPEN_NI_INCLUDE}")
FIND_PATH(OPEN_NI_INCLUDE "XnOpenNI.h" HINTS "$ENV{OPEN_NI_INCLUDE}" "/usr/include/ni" "/usr/include/openni")
FIND_LIBRARY(OPEN_NI_LIBRARY NAMES OpenNI libOpenNI HINTS $ENV{OPEN_NI_LIB} "/usr/lib")
LINK_DIRECTORIES($ENV{OPEN_NI_LIB})
INCLUDE_DIRECTORIES(${OPEN_NI_INCLUDE})
LINK_LIBRARIES(${OPEN_NI_LIBRARY})

#NITE Lib MESSAGE("$ENV{XN_NITE_INSTALL_PATH}")
FIND_PATH(XN_NITE_INCLUDE "XnVNite.h" HINTS "$ENV{XN_NITE_INSTALL_PATH}Include" "/usr/include/nite")
FIND_LIBRARY(XN_NITE_LIBRARY NAMES XnVNITE libXnVNITE libXnVNITE_1_3_1 libXnVNite_1_3_1 libXnVNite_1_3_1.so XnVNITE_1_3_1 HINTS $ENV{XN_NITE_INSTALL_PATH}Lib "/usr/lib")
LINK_DIRECTORIES($ENV{XN_NITE_INSTALL_PATH}Lib)
INCLUDE_DIRECTORIES(${XN_NITE_INCLUDE})
LINK_LIBRARIES(${XN_NITE_LIBRARY})

# Search for source code.
FILE(GLOB folder_source *.cpp *.cc *.c)
FILE(GLOB folder_header *.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

# Automatically add include directories if needed.
FOREACH(header_file ${folder_header})
  GET_FILENAME_COMPONENT(p ${header_file} PATH)
ENDFOREACH(header_file ${folder_header})

# Set up our main executable.
IF (folder_source)
  ADD_EXECUTABLE(DeviceTest ${folder_source} ${folder_header})
ELSE (folder_source)
  MESSAGE(FATAL_ERROR "No source code files found. Please add something")
ENDIF (folder_source)


TARGET_LINK_LIBRARIES(DeviceTest ${CORELIBS} ${OPENGL_LIBRARY} ${GLUT_LIBRARY} ${LINK_LIBRARIES} ${YARP_LIBRARIES})
