# Copyright (C) 2011 Duarte Aragao
# Author: Duarte Aragao
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Search for source code.
file(GLOB folder_source *.cpp *.cc *.c)
file(GLOB folder_header *.h)

# Automatically add include directories if needed.
foreach(header_file ${folder_header})
  get_filename_component(p ${header_file} PATH)
endforeach(header_file ${folder_header})

yarp_add_plugin(KinectYarpDeviceClientLib ${folder_source} ${folder_header})
target_link_libraries(KinectYarpDeviceClientLib YARP::YARP_OS
                                                YARP::YARP_sig
                                                YARP::YARP_dev)
yarp_install(TARGETS KinectYarpDeviceClientLib
             EXPORT YARP
             COMPONENT runtime
             LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
             ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

set_property(TARGET KinectYarpDeviceClientLib PROPERTY FOLDER "Unsupported")
