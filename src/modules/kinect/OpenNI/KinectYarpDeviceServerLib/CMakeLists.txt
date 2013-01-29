# Copyright (C) 2011 Duarte Aragao
# Author: Duarte Aragao
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

 #OPENNI Lib MESSAGE("$ENV{OPEN_NI_INCLUDE}")
#ONLY TESTED WITH THE ROS OPENNI
FIND_PATH(OPEN_NI_INCLUDE_LOCAL NAMES XnOpenNI XnOpenNI.h HINTS $ENV{OPEN_NI_INCLUDE} /usr/include/openni /usr/include/ni)
FIND_LIBRARY(OPEN_NI_LIBRARY NAMES OpenNI libOpenNI libOpenNI.so HINTS $ENV{OPEN_NI_LIB} "/usr/lib")
LINK_DIRECTORIES($ENV{OPEN_NI_LIB})
INCLUDE_DIRECTORIES(${OPEN_NI_INCLUDE_LOCAL})
LINK_LIBRARIES(${OPEN_NI_LIBRARY})

#MESSAGE("OpenNI\nlib:${OPEN_NI_LIBRARY_DIR} ${OPEN_NI_LIBRARY}\ninc:${OPEN_NI_INCLUDE_LOCAL}\n")

#NITE Lib MESSAGE("$ENV{XN_NITE_INSTALL_PATH}")
FIND_PATH(XN_NITE_INCLUDE NAMES "XnVNite" "XnVNite.h" HINTS "$ENV{XN_NITE_INSTALL_PATH}Include" "/usr/include/nite")
FIND_LIBRARY(XN_NITE_LIBRARY NAMES XnVNITE libXnVNite  libXnVNite_1_3_1  libXnVNite_1_3_1.so libXnVNite.so HINTS $ENV{XN_NITE_INSTALL_PATH} Lib "/usr/lib")
LINK_DIRECTORIES($ENV{XN_NITE_INSTALL_PATH})
INCLUDE_DIRECTORIES(${XN_NITE_INCLUDE})
LINK_LIBRARIES(${XN_NITE_LIBRARY})

#MESSAGE("\nNite\nlib:${XN_NITE_LIBRARY_DIR} ${XN_NITE_LIBRARY}\ninc:${XN_NITE_INCLUDE}")

# Search for source code.
FILE(GLOB folder_source *.cpp *.cc *.c)
FILE(GLOB folder_header *.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

  

# Automatically add include directories if needed.
FOREACH(header_file ${folder_header})
  GET_FILENAME_COMPONENT(p ${header_file} PATH)
ENDFOREACH(header_file ${folder_header})

YARP_ADD_PLUGIN (KinectYarpDeviceServerLib ${folder_source} ${folder_header})
TARGET_LINK_LIBRARIES(KinectYarpDeviceServerLib ${YARP_LIBRARIES} ${LINK_LIBRARIES})
