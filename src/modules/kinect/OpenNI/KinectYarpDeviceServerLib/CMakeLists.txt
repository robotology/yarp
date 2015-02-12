# Copyright (C) 2011 Duarte Aragao
# Author: Duarte Aragao
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

find_package(YARP REQUIRED)
include_directories(${YARP_INCLUDE_DIRS})

#OPENNI Lib
# message("$ENV{OPEN_NI_INCLUDE}")
#ONLY TESTED WITH THE ROS OPENNI
find_path(OPEN_NI_INCLUDE_LOCAL NAMES XnOpenNI XnOpenNI.h
                                HINTS $ENV{OPEN_NI_INCLUDE} /usr/include/openni /usr/include/ni)
find_library(OPEN_NI_LIBRARY NAMES OpenNI libOpenNI libOpenNI.so
                             HINTS $ENV{OPEN_NI_LIB} "/usr/lib")
link_directories($ENV{OPEN_NI_LIB})
include_directories(${OPEN_NI_INCLUDE_LOCAL})
link_libraries(${OPEN_NI_LIBRARY})
#message("OpenNI\nlib:${OPEN_NI_LIBRARY_DIR} ${OPEN_NI_LIBRARY}\ninc:${OPEN_NI_INCLUDE_LOCAL}\n")


#NITE Lib
# message("$ENV{XN_NITE_INSTALL_PATH}")
find_path(XN_NITE_INCLUDE NAMES "XnVNite" "XnVNite.h"
                          HINTS "$ENV{XN_NITE_INSTALL_PATH}Include" "/usr/include/nite")
find_library(XN_NITE_LIBRARY NAMES XnVNITE libXnVNite  libXnVNite_1_3_1  libXnVNite_1_3_1.so libXnVNite.so
                             HINTS $ENV{XN_NITE_INSTALL_PATH} Lib "/usr/lib")
link_directories($ENV{XN_NITE_INSTALL_PATH})
include_directories(${XN_NITE_INCLUDE})
link_libraries(${XN_NITE_LIBRARY})
#message("\nNite\nlib:${XN_NITE_LIBRARY_DIR} ${XN_NITE_LIBRARY}\ninc:${XN_NITE_INCLUDE}")

# Search for source code.
file(GLOB folder_source *.cpp *.cc *.c)
file(GLOB folder_header *.h)
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})


# Automatically add include directories if needed.
foreach(header_file ${folder_header})
  get_filename_component(p ${header_file} PATH)
endforeach(header_file ${folder_header})

yarp_add_plugin(KinectYarpDeviceServerLib ${folder_source} ${folder_header})
target_link_libraries(KinectYarpDeviceServerLib ${YARP_LIBRARIES} ${LINK_LIBRARIES})
