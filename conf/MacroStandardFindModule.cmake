# - Try to find a package using a cmake config file, or pkgconfig
#
# MACRO_STANDARD_FIND_MODULE( <name>
#                             <pkgconfig name>
#                             [NOT_REQUIRED]
#                             [SKIP_CMAKE_CONFIG]
#                             [SKIP_PKG_CONFIG] )
#
# If the package is found, the following variables (where possible)
# are created:
#
#  <name>_FOUND         - System has <name>
#  <name>_INCLUDE_DIRS  - <name> include directory
#  <name>_LIBRARIES     - <name> libraries
#  <name>_DEFINITIONS   - Additional compiler flags for <name>
#  <name>_VERSION       - <name> version
#  <name>_MAJOR_VERSION - <name> major version
#  <name>_MINOR_VERSION - <name> minor version
#  <name>_PATCH_VERSION - <name> patch version
#  <name>_TWEAK_VERSION - <name> tweak version
#
# For each library that requires to be linked (i.e. "-llib") it creates
#  <name>_<LIB>_LIBRARY_RELEASE (cached, advanced)
#  <name>_<LIB>_LIBRARY_DEBUG (cached, advanced, and empty by default)
#  <name>_<LIB>_LIBRARY
#  <name>_<LIB>_LIBRARY_FOUND
#
# In a FindXXX.cmake module, this macro can be used at the beginning.
# The NOT_REQUIRED can be added to avoid failing if the package was not
# found, but pkg-config is installed.
# If <name>_FOUND is FALSE at the end, more "custom" searches can be
# used (for windows, etc.)
#
# If SKIP_CMAKE_CONFIG or SKIP_PKG_CONFIG are set, the relative step
# is skipped
#
# If one of the variables
#  MACRO_STANDARD_FIND_MODULE_DEBUG
#  MACRO_STANDARD_FIND_MODULE_DEBUG_<name>
# is set to TRUE, prints more useful debug output

# Copyright (C) 2012, 2013  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


include(FindPackageHandleStandardArgs)
include(CMakeParseArguments)
include(SelectLibraryConfigurations)
include(MacroExtractVersion)

macro(MACRO_STANDARD_FIND_MODULE _name _pkgconfig_name)
    string(TOUPPER ${_name} _NAME)
    cmake_parse_arguments(_OPT_${_NAME} "NOT_REQUIRED;SKIP_CMAKE_CONFIG;SKIP_PKG_CONFIG" "" "" ${ARGN})

    # Try to use CMake Config file to locate the package
    if(NOT _OPT_${_NAME}_SKIP_CMAKE_CONFIG)
        set(_${_name}_FIND_QUIETLY ${${_name}_FIND_QUIETLY})
        find_package(${_name} QUIET NO_MODULE)
        set(${_name}_FIND_QUIETLY ${_${_name}_FIND_QUIETLY})
        mark_as_advanced(${_name}_DIR)

        if(${_name}_FOUND)
            find_package_handle_standard_args(${_name} DEFAULT_MSG ${_name}_CONFIG)
        endif()
    endif()

    if(NOT ${_name}_FOUND AND NOT _OPT_${_NAME}_SKIP_PKG_CONFIG)
        # No CMake Config file was found. Try using PkgConfig
        find_package(PkgConfig QUIET)
        if(PKG_CONFIG_FOUND)

            if(${_name}_FIND_VERSION)
                if(${_name}_FIND_VERSION_EXACT)
                    pkg_check_modules(_PC_${_NAME} QUIET ${_pkgconfig_name}=${${_name}_FIND_VERSION})
                else(${_name}_FIND_VERSION_EXACT)
                    pkg_check_modules(_PC_${_NAME} QUIET ${_pkgconfig_name}>=${${_name}_FIND_VERSION})
                endif(${_name}_FIND_VERSION_EXACT)
            else(${_name}_FIND_VERSION)
                pkg_check_modules(_PC_${_NAME} QUIET ${_pkgconfig_name})
            endif(${_name}_FIND_VERSION)


            if(_PC_${_NAME}_FOUND)
                set(${_name}_INCLUDE_DIRS ${_PC_${_NAME}_INCLUDE_DIRS} CACHE PATH "${_name} include directory")
                set(${_name}_DEFINITIONS ${_PC_${_NAME}_CFLAGS_OTHER} CACHE STRING "Additional compiler flags for ${_name}")

                set(${_name}_LIBRARIES)
                foreach(_library IN ITEMS ${_PC_${_NAME}_LIBRARIES})
                    string(TOUPPER ${_library} _LIBRARY)
                    find_library(${_name}_${_LIBRARY}_LIBRARY_RELEASE
                                 NAMES ${_library}
                                 PATHS ${_PC_${_NAME}_LIBRARY_DIRS})
                    list(APPEND ${_name}_LIBRARIES ${${_name}_${_LIBRARY}_LIBRARY_RELEASE})
                    select_library_configurations(${_name}_${_LIBRARY})
                    if(MACRO_STANDARD_FIND_MODULE_DEBUG OR MACRO_STANDARD_FIND_MODULE_DEBUG_${_name})
                        message(STATUS "${_name}_${_LIBRARY}_FOUND = ${${_name}_${_LIBRARY}_FOUND}")
                        message(STATUS "${_name}_${_LIBRARY}_LIBRARY_RELEASE = ${${_name}_${_LIBRARY}_LIBRARY_RELEASE}")
                        message(STATUS "${_name}_${_LIBRARY}_LIBRARY_DEBUG = ${${_name}_${_LIBRARY}_LIBRARY_DEBUG}")
                        message(STATUS "${_name}_${_LIBRARY}_LIBRARY = ${${_name}_${_LIBRARY}_LIBRARY}")
                    endif()
                endforeach()

                set(${_name}_VERSION ${_PC_${_NAME}_VERSION})

            endif(_PC_${_NAME}_FOUND)

            mark_as_advanced(${_name}_INCLUDE_DIRS
                             ${_name}_DEFINITIONS)

            # If NOT_REQUIRED unset the _FIND_REQUIRED variable and save it for later
            if(_OPT_${_NAME}_NOT_REQUIRED AND DEFINED ${_name}_FIND_REQUIRED)
                set(_${_name}_FIND_REQUIRED ${${_name}_FIND_REQUIRED})
                set(_${_name}_FIND_QUIETLY ${${_name}_FIND_QUIETLY})
                unset(${_name}_FIND_REQUIRED)
                set(${_name}_FIND_QUIETLY 1)
            endif()

            find_package_handle_standard_args(${_name} DEFAULT_MSG ${_name}_LIBRARIES)

            # If NOT_REQUIRED reset the _FIND_REQUIRED variable
            if(_OPT_${_NAME}_NOT_REQUIRED AND DEFINED _${_name}_FIND_REQUIRED)
                set(${_name}_FIND_REQUIRED ${_${_name}_FIND_REQUIRED})
                set(${_name}_FIND_QUIETLY ${_${_name}_FIND_QUIETLY})
            endif()

        endif()
    endif()

    # ${_name}_FOUND is uppercase after find_package_handle_standard_args
    set(${_name}_FOUND ${${_NAME}_FOUND})

    # Extract version numbers
    if(${_name}_FOUND)
        macro_extract_version(${_name})
    endif()


    # Print some debug output if either MACRO_STANDARD_FIND_MODULE_DEBUG
    # or MACRO_STANDARD_FIND_MODULE_DEBUG_${_name} is set to TRUE
    if(MACRO_STANDARD_FIND_MODULE_DEBUG OR MACRO_STANDARD_FIND_MODULE_DEBUG_${_name})
        message(STATUS "${_name}_FOUND = ${${_name}_FOUND}")
        message(STATUS "${_name}_INCLUDE_DIRS = ${${_name}_INCLUDE_DIRS}")
        message(STATUS "${_name}_LIBRARIES = ${${_name}_LIBRARIES}")
        message(STATUS "${_name}_DEFINITIONS = ${${_name}_DEFINITIONS}")
        message(STATUS "${_name}_VERSION = ${${_name}_VERSION}")
        message(STATUS "${_name}_MAJOR_VERSION = ${${_name}_MAJOR_VERSION}")
        message(STATUS "${_name}_MINOR_VERSION = ${${_name}_MINOR_VERSION}")
        message(STATUS "${_name}_PATCH_VERSION = ${${_name}_PATCH_VERSION}")
        message(STATUS "${_name}_TWEAK_VERSION = ${${_name}_TWEAK_VERSION}")
    endif()

endmacro()
