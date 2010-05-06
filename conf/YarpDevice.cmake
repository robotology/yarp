## Skip this whole file if it has already been included
IF (NOT COMMAND END_PLUGIN_LIBRARY)

# "Device" libraries have been replaced with "Plugin" libraries
INCLUDE(YarpPlugin)

#########################################################################
## Deprecated functions

MACRO(TARGET_IMPORT_DEVICES target hdr)
  MESSAGE(STATUS "[TARGET_]IMPORT_DEVICES macro has been deprecated")
  MESSAGE(STATUS "Instead just link against the device library, and in code do:")
  MESSAGE(STATUS "#include <yarp/dev/all.h>")
  MESSAGE(STATUS "YARP_DECLARE_DEVICES(${target});")
  MESSAGE(STATUS "...")
  MESSAGE(STATUS "   Network yarp;")
  MESSAGE(STATUS "   YARP_REGISTER_DEVICES(${devices});")
ENDMACRO(TARGET_IMPORT_DEVICES target hdr)


#MACRO(IMPORT_DEVICES hdr)
#  TARGET_IMPORT_DEVICES(the_device_library_name ${hdr})
#ENDMACRO(IMPORT_DEVICES hdr)

MACRO(END_DEVICE_LIBRARY bundle_name)
  END_PLUGIN_LIBRARY(${bundle_name})
  ADD_PLUGIN_LIBRARY_EXECUTABLE(${bundle_name}dev ${bundle_name})
ENDMACRO(END_DEVICE_LIBRARY)

MACRO(IMPORT_DEVICES hdr bundle_name)
  SET(libname ${bundle_name})
  CONFIGURE_FILE(${YARP_MODULE_PATH}/template/yarpdev_compat_plugin.h
    ${hdr} @ONLY IMMEDIATE)
  CONFIGURE_FILE(${YARP_MODULE_PATH}/template/yarpdev_compat_plugin.cmake
    ${CMAKE_BINARY_DIR}/${bundle_name}Config.cmake @ONLY IMMEDIATE)
  #SET(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/old_conf ${CMAKE_MODULE_PATH})
ENDMACRO(IMPORT_DEVICES)

ENDIF (NOT COMMAND END_PLUGIN_LIBRARY)

