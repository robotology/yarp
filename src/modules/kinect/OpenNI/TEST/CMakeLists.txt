# Copyright (C) 2011 Duarte Aragao
# Author: Duarte Aragao
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
cmake_minimum_required(VERSION 2.8.7)

find_package(YARP REQUIRED)
link_libraries(${YARP_LIBRARIES})
include_directories(${YARP_INCLUDE_DIRS})

find_package(GLUT)
find_package(OpenGL)

 #OPENNI Lib MESSAGE("$ENV{OPEN_NI_INCLUDE}")
find_path(OPEN_NI_INCLUDE "XnOpenNI.h" HINTS "$ENV{OPEN_NI_INCLUDE}" "/usr/include/ni" "/usr/include/openni")
find_library(OPEN_NI_LIBRARY NAMES OpenNI libOpenNI HINTS $ENV{OPEN_NI_LIB} "/usr/lib")
link_directories($ENV{OPEN_NI_LIB})
include_directories(${OPEN_NI_INCLUDE})
link_libraries(${OPEN_NI_LIBRARY})

#NITE Lib MESSAGE("$ENV{XN_NITE_INSTALL_PATH}")
find_path(XN_NITE_INCLUDE "XnVNite.h" HINTS "$ENV{XN_NITE_INSTALL_PATH}Include" "/usr/include/nite")
find_library(XN_NITE_LIBRARY NAMES XnVNITE libXnVNITE libXnVNITE_1_3_1 libXnVNite_1_3_1 libXnVNite_1_3_1.so XnVNITE_1_3_1 HINTS $ENV{XN_NITE_INSTALL_PATH}Lib "/usr/lib")
link_directories($ENV{XN_NITE_INSTALL_PATH}Lib)
include_directories(${XN_NITE_INCLUDE})
link_libraries(${XN_NITE_LIBRARY})

# Search for source code.
file(GLOB folder_source *.cpp *.cc *.c)
file(GLOB folder_header *.h)
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

# Automatically add include directories if needed.
foreach(header_file ${folder_header})
  get_filename_component(p ${header_file} PATH)
endforeach(header_file ${folder_header})

# Set up our main executable.
if(folder_source)
  add_executable(DeviceTest ${folder_source} ${folder_header})
else(folder_source)
  message(FATAL_ERROR "No source code files found. Please add something")
endif (folder_source)

target_link_libraries(DeviceTest ${CORELIBS} ${OPENGL_LIBRARY} ${GLUT_LIBRARY} ${LINK_LIBRARIES} ${YARP_LIBRARIES})
