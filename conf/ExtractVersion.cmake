#.rst:
# ExtractVersion
# --------------
#
# Extracts version numbers from a version string::
#
#  extract_version (<name> [REVERSE_NAME])
#
#
# Tries to extract the following variables (the second version is used
# if REVERSE_NAME is set as argument)::
#
#   <name>_MAJOR_VERSION or <name>_VERSION_MAJOR - <name> major version
#   <name>_MINOR_VERSION or <name>_VERSION_MINOR - <name> minor version
#   <name>_PATCH_VERSION or <name>_VERSION_PATCH - <name> patch version
#   <name>_TWEAK_VERSION or <name>_VERSION_TWEAK - <name> tweak version
#   <name>_VERSION_COUNT - number of version components, 0 to 4

#=============================================================================
# Copyright 2013 iCub Facility, Istituto Italiano di Tecnologia
#   Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)


if(DEFINED __EXTRACT_VERSION_INCLUDED)
  return()
endif()
set(__EXTRACT_VERSION_INCLUDED TRUE)


macro(EXTRACT_VERSION _name)
    if("x${ARGV2}" STREQUAL "xREVERSE_NAME")
        set(_reverse 1)
    endif()

    if(${_name}_VERSION)
        string(REPLACE "." ";" _version_list ${${_name}_VERSION})
        list(LENGTH _version_list _version_size)
        if (_version_size GREATER 0)
            if(NOT _reverse)
                list(GET _version_list 0 ${_name}_MAJOR_VERSION)
            else()
                list(GET _version_list 0 ${_name}_VERSION_MAJOR)
            endif()
        endif()
        if (_version_size GREATER 1)
            if(NOT _reverse)
                list(GET _version_list 1 ${_name}_MINOR_VERSION)
            else()
                list(GET _version_list 1 ${_name}_VERSION_MINOR)
            endif()
        endif()
        if (_version_size GREATER 2)
            if(NOT _reverse)
                list(GET _version_list 2 ${_name}_PATCH_VERSION)
            else()
                list(GET _version_list 2 ${_name}_VERSION_PATCH)
            endif()
        endif()
        if (_version_size GREATER 3)
            if(NOT _reverse)
                list(GET _version_list 3 ${_name}_TWEAK_VERSION)
            else()
                list(GET _version_list 3 ${_name}_VERSION_TWEAK)
            endif()
        endif()

        if(${_version_size} GREATER 4)
            set(${_name}_VERSION_COUNT 4)
        else()
            set(${_name}_VERSION_COUNT ${_version_size})
        endif()
    else()
        set(${_name}_VERSION_COUNT 0)
    endif()

    unset(_version_list)
    unset(_version_size)
endmacro()
