# Copyright: (C) 2009, 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

## Skip this whole file if it has already been included
if(NOT COMMAND YARP_END_PLUGIN_LIBRARY)

# "Device" libraries have been replaced with "Plugin" libraries
include(YarpPlugin)

#########################################################################
## Deprecated functions

macro(TARGET_IMPORT_DEVICES target hdr)
    message(STATUS "[TARGET_]IMPORT_DEVICES macro has been deprecated")
    message(STATUS "Instead just link against the device library, and in code do:")
    message(STATUS "#include <yarp/os/all.h>")
    message(STATUS "YARP_DECLARE_PLUGINS(${target});")
    message(STATUS "...")
    message(STATUS "   Network yarp;")
    message(STATUS "   YARP_REGISTER_PLUGINS(${target});")
endmacro(TARGET_IMPORT_DEVICES target hdr)

macro(END_DEVICE_LIBRARY bundle_name)
    yarp_end_plugin_library(${bundle_name})
    add_plugin_library_executable(${bundle_name}dev ${bundle_name})
    if(MSVC)
        set_target_properties(${bundle_name} PROPERTIES DEBUG_POSTFIX "d")
    endif(MSVC)
endmacro(END_DEVICE_LIBRARY)

macro(IMPORT_DEVICES hdr bundle_name)
    set(libname ${bundle_name})
    configure_file(${YARP_MODULE_DIR}/template/yarpdev_compat_plugin.h
                   ${hdr} @ONLY IMMEDIATE)
    configure_file(${YARP_MODULE_DIR}/template/yarpdev_compat_plugin.cmake
                   ${CMAKE_BINARY_DIR}/${bundle_name}Config.cmake @ONLY IMMEDIATE)
endmacro(IMPORT_DEVICES)

endif(NOT COMMAND YARP_END_PLUGIN_LIBRARY)
