# Copyright (C) 2011 Duarte Aragao
# Author: Duarte Aragao
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
include_directories(${YARP_OS_INCLUDE_DIRS}
                    ${YARP_sig_INCLUDE_DIRS}
                    ${YARP_dev_INCLUDE_DIRS})

# Search for source code.
file(GLOB folder_source *.cpp *.cc *.c)
file(GLOB folder_header *.h)
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

# Automatically add include directories if needed.
foreach(header_file ${folder_header})
  get_filename_component(p ${header_file} PATH)
endforeach(header_file ${folder_header})

yarp_add_plugin(KinectYarpDeviceClientLib ${folder_source} ${folder_header})
target_link_libraries(KinectYarpDeviceClientLib YARP_OS
                                                YARP_sig
                                                YARP_dev)
install(TARGETS KinectYarpDeviceClientLib
        COMPONENT runtime
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
