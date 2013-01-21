# Copyright: (C) 2009, 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

## Skip this whole file if it has already been included
IF (NOT COMMAND YARP_END_PLUGIN_LIBRARY)

# "Device" libraries have been replaced with "Plugin" libraries
INCLUDE(YarpPlugin)

#########################################################################
## Deprecated functions

MACRO(TARGET_IMPORT_DEVICES target hdr)
  MESSAGE(STATUS "[TARGET_]IMPORT_DEVICES macro has been deprecated")
  MESSAGE(STATUS "Instead just link against the device library, and in code do:")
  MESSAGE(STATUS "#include <yarp/os/all.h>")
  MESSAGE(STATUS "YARP_DECLARE_PLUGINS(${target});")
  MESSAGE(STATUS "...")
  MESSAGE(STATUS "   Network yarp;")
  MESSAGE(STATUS "   YARP_REGISTER_PLUGINS(${target});")
ENDMACRO(TARGET_IMPORT_DEVICES target hdr)

MACRO(END_DEVICE_LIBRARY bundle_name)
  YARP_END_PLUGIN_LIBRARY(${bundle_name})
  ADD_PLUGIN_LIBRARY_EXECUTABLE(${bundle_name}dev ${bundle_name})
  IF (MSVC)
    SET_TARGET_PROPERTIES(${bundle_name} PROPERTIES DEBUG_POSTFIX "d")
  ENDIF (MSVC)
ENDMACRO(END_DEVICE_LIBRARY)

MACRO(IMPORT_DEVICES hdr bundle_name)
  SET(libname ${bundle_name})
  CONFIGURE_FILE(${YARP_MODULE_PATH}/template/yarpdev_compat_plugin.h
    ${hdr} @ONLY IMMEDIATE)
  CONFIGURE_FILE(${YARP_MODULE_PATH}/template/yarpdev_compat_plugin.cmake
    ${CMAKE_BINARY_DIR}/${bundle_name}Config.cmake @ONLY IMMEDIATE)
ENDMACRO(IMPORT_DEVICES)

ENDIF (NOT COMMAND YARP_END_PLUGIN_LIBRARY)

