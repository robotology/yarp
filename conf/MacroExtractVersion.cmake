# - Extracts version numbers from a version string
#
# MACRO_EXTRACT_VERSION(<name> <version_string>)
# Tries to extract the following variables
#  <name>_MAJOR_VERSION - <name> major version
#  <name>_MINOR_VERSION - <name> minor version
#  <name>_PATCH_VERSION - <name> patch version
#  <name>_TWEAK_VERSION - <name> tweak version

# Copyright (C) 2013  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

macro(MACRO_EXTRACT_VERSION _name)
    if(${_name}_VERSION)
        string(REPLACE "." ";" X_${_name}_VERSION_LIST ${${_name}_VERSION})
        list(LENGTH X_${_name}_VERSION_LIST X_${_name}_VERSION_SIZE)
        if (X_${_name}_VERSION_SIZE GREATER 0)
            list(GET X_${_name}_VERSION_LIST 0 ${_name}_MAJOR_VERSION)
        endif()
        if (X_${_name}_VERSION_SIZE GREATER 1)
            list(GET X_${_name}_VERSION_LIST 1 ${_name}_MINOR_VERSION)
        endif()
        if (X_${_name}_VERSION_SIZE GREATER 2)
            list(GET X_${_name}_VERSION_LIST 2 ${_name}_PATCH_VERSION)
        endif()
        if (X_${_name}_VERSION_SIZE GREATER 3)
            list(GET X_${_name}_VERSION_LIST 3 ${_name}_TWEAK_VERSION)
        endif()
    endif()
endmacro()
