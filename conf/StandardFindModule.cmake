#.rst:
# StandardFindModule
# ------------------
#
# Try to find a package using a cmake config file, or pkgconfig::
#
#   standard_find_module(<name>
#                        <pkgconfig name>
#                        [NOT_REQUIRED]
#                        [QUIET]
#                        [SKIP_CMAKE_CONFIG]
#                        [SKIP_PKG_CONFIG]
#                        [TARGET <target>]
#                        [REPLACE_TARGETS <target> [...]]
#   )
#
# If the package is found, the following variables (where possible)
# are created::
#
#   <name>_FOUND         - System has <name>
#   <name>_INCLUDE_DIRS  - <name> include directory
#   <name>_LIBRARIES     - <name> libraries
#   <name>_DEFINITIONS   - Additional compiler flags for <name>
#   <name>_VERSION       - <name> version
#   <name>_MAJOR_VERSION - <name> major version
#   <name>_MINOR_VERSION - <name> minor version
#   <name>_PATCH_VERSION - <name> patch version
#   <name>_TWEAK_VERSION - <name> tweak version
#   <name>_VERSION_COUNT - Number of version components, 0 to 4
#
# For each library that requires to be linked (i.e. ``-llib``) it
# creates::
#
#   <name>_<LIB>_LIBRARY_RELEASE (cached, advanced)
#   <name>_<LIB>_LIBRARY_DEBUG (cached, advanced, and empty by default)
#   <name>_<LIB>_LIBRARY
#   <name>_<LIB>_LIBRARY_FOUND
#
# In a ``FindXXX.cmake`` module, this macro can be used at the beginning.
# The ``NOT_REQUIRED`` can be added to avoid failing if the package was not
# found, but ``pkg-config`` is installed.
# The ``QUIET`` argument can be used to hide the output from
# `find_package_handle_standard_args`.
# If ``<name>_FOUND`` is ``FALSE`` at the end, more "custom" searches can be
# used (for windows, etc.)
#
# If ``SKIP_CMAKE_CONFIG`` or ``SKIP_PKG_CONFIG`` are set, the relative step
# is skipped
#
# If ``TARGET`` is specified, in ``pkg-config`` mode, an imported target will
# be created using the first library returned by ``pkg-config`` as imported
# location.
#
# The ``REPLACE_TARGETS`` can be used to pass a list of imported targets that
# will be detected in variables and target properties, and replaced with the
# corresponding target.
#
# If one of the ``STANDARD_FIND_MODULE_USE_IMPORTED_TARGET`` or
# ``STANDARD_FIND_MODULE_USE_IMPORTED_TARGET_<name>`` are enabled, and
# a ``TARGET`` is specified, the ``<name>_LIBRARIES`` variable content is
# replaced with the imported target.
#
# If one of the variables ``STANDARD_FIND_MODULE_DEBUG`` or
# ``STANDARD_FIND_MODULE_DEBUG_<name>`` is enabled, prints more useful debug
# output

#=============================================================================
# Copyright 2012-2013 iCub Facility, Istituto Italiano di Tecnologia
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


if(DEFINED __STANDARD_FIND_MODULE_INCLUDED)
  return()
endif()
set(__STANDARD_FIND_MODULE_INCLUDED TRUE)


include(FindPackageHandleStandardArgs)
include(CMakeParseArguments)
include(SelectLibraryConfigurations)
include(ExtractVersion)
include(ReplaceImportedTargets)

macro(STANDARD_FIND_MODULE _name _pkgconfig_name)
  string(TOUPPER ${_name} _NAME)
  cmake_parse_arguments(_OPT_${_NAME} "NOT_REQUIRED;SKIP_CMAKE_CONFIG;SKIP_PKG_CONFIG;QUIET" "TARGET" "REPLACE_TARGETS" ${ARGN})

  # Try to use CMake Config file to locate the package
  if(NOT _OPT_${_NAME}_SKIP_CMAKE_CONFIG)
    # Disable package cache when not done automatically by CMake
    # This is a workaround for CMake bug #14849
    unset(_standard_find_module_registryArgs)
    if(NOT CMAKE_REQUIRED_VERSION VERSION_LESS 3.1)
      message(AUTHOR_WARNING "Disabling cmake cache is supported since CMake 3.1. You can remove this check")
    endif()
    if(CMAKE_VERSION VERSION_LESS 3.1)
      if(CMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY)
        list(APPEND _standard_find_module_registryArgs NO_CMAKE_PACKAGE_REGISTRY)
      endif()
      if(CMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY)
        list(APPEND _standard_find_module_registryArgs NO_CMAKE_SYSTEM_PACKAGE_REGISTRY)
      endif()
    endif()

    set(_${_name}_FIND_QUIETLY ${${_name}_FIND_QUIETLY})
    find_package(${_name} QUIET NO_MODULE ${_standard_find_module_registryArgs})
    set(${_name}_FIND_QUIETLY ${_${_name}_FIND_QUIETLY})
    mark_as_advanced(${_name}_DIR)

    if(${_name}_FOUND)
      set(_${_name}_FIND_QUIETLY ${${_name}_FIND_QUIETLY})
      set(${_name}_FIND_QUIETLY ${_OPT_${_NAME}_QUIET})
      find_package_handle_standard_args(${_name} DEFAULT_MSG ${_name}_CONFIG)
      set(${_name}_FIND_QUIETLY ${_${_name}_FIND_QUIETLY})
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
        unset(_include_dirs)
        foreach(_dir ${_PC_${_NAME}_INCLUDE_DIRS})
          if(EXISTS "${_dir}")
            list(APPEND _include_dirs "${_dir}")
          endif()
        endforeach()
        set(${_name}_INCLUDE_DIRS ${_include_dirs} CACHE PATH "${_name} include directory")
        unset(_include_dirs)
        set(${_name}_DEFINITIONS ${_PC_${_NAME}_CFLAGS_OTHER} CACHE STRING "Additional compiler flags for ${_name}")

        set(${_name}_LIBRARIES)

        foreach(_library IN ITEMS ${_PC_${_NAME}_LIBRARIES})
          string(TOUPPER ${_library} _LIBRARY)
          find_library(${_name}_${_LIBRARY}_LIBRARY_RELEASE
                       NAMES ${_library}
                       PATHS ${_PC_${_NAME}_LIBRARY_DIRS})
          list(APPEND ${_name}_LIBRARIES ${${_name}_${_LIBRARY}_LIBRARY_RELEASE})
          select_library_configurations(${_name}_${_LIBRARY})
          if(STANDARD_FIND_MODULE_DEBUG OR STANDARD_FIND_MODULE_DEBUG_${_name})
            message(STATUS "${_name}_${_LIBRARY}_FOUND = ${${_name}_${_LIBRARY}_FOUND}")
            message(STATUS "${_name}_${_LIBRARY}_LIBRARY_RELEASE = ${${_name}_${_LIBRARY}_LIBRARY_RELEASE}")
            message(STATUS "${_name}_${_LIBRARY}_LIBRARY_DEBUG = ${${_name}_${_LIBRARY}_LIBRARY_DEBUG}")
            message(STATUS "${_name}_${_LIBRARY}_LIBRARY = ${${_name}_${_LIBRARY}_LIBRARY}")
          endif()

          # Create imported target (only for the first library
          # returned by pkg-config
          if(DEFINED _OPT_${_NAME}_TARGET)
            if(NOT TARGET ${_OPT_${_NAME}_TARGET})
              add_library(${_OPT_${_NAME}_TARGET} UNKNOWN IMPORTED)

              if(${_name}_${_LIBRARY}_LIBRARY_RELEASE)
                set_property(TARGET ${_OPT_${_NAME}_TARGET} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
                set_property(TARGET ${_OPT_${_NAME}_TARGET}        PROPERTY IMPORTED_LOCATION_RELEASE "${${_name}_${_LIBRARY}_LIBRARY_RELEASE}" )
              endif()

              if(${_name}_${_LIBRARY}_LIBRARY_DEBUG)
                set_property(TARGET ${_OPT_${_NAME}_TARGET} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
                set_property(TARGET ${_OPT_${_NAME}_TARGET}        PROPERTY IMPORTED_LOCATION_DEBUG "${${_name}_${_LIBRARY}_LIBRARY_DEBUG}" )
              endif()

              if(${_name}_${_LIBRARY}_INCLUDE_DIRS)
                set_property(TARGET ${_OPT_${_NAME}_TARGET} APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${${_name}_${_LIBRARY}_INCLUDE_DIRS}")
              endif()

              if(${_name}_DEFINITIONS)
                set_property(TARGET ${_OPT_${_NAME}_TARGET} PROPERTY INTERFACE_COMPILE_DEFINITIONS "${GTK2_DEFINITIONS}")
              endif()
            else()
              get_property(_configurations TARGET ${_OPT_${_NAME}_TARGET} PROPERTY IMPORTED_CONFIGURATIONS)
              foreach(_config ${_configurations})
                if(${_name}_${_LIBRARY}_LIBRARY_${_config})
                  set_property(TARGET ${_OPT_${_NAME}_TARGET} APPEND PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_${_config} "${${_name}_${_LIBRARY}_LIBRARY_${_config}}")
                elseif(${_name}_${_LIBRARY}_LIBRARY_RELEASE)
                  set_property(TARGET ${_OPT_${_NAME}_TARGET} APPEND PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_${_config} "${${_name}_${_LIBRARY}_LIBRARY_RELEASE}")
                elseif(${_name}_${_LIBRARY}_LIBRARY_DEBUG)
                  set_property(TARGET ${_OPT_${_NAME}_TARGET} APPEND PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_${_config} "${${_name}_${_LIBRARY}_LIBRARY_DEBUG}")
                endif()
              endforeach()
            endif()
          endif()
        endforeach()

        replace_imported_targets(${_name}_LIBRARIES ${_OPT_${_NAME}_REPLACE_TARGETS})
        if(DEFINED _OPT_${_NAME}_TARGET)
          replace_imported_targets(${_OPT_${_NAME}_TARGET} ${_OPT_${_NAME}_REPLACE_TARGETS})
          # If requested, replace the <NAME>_LIBRARIES variable
          # content with the corresponding imported target.
          if(STANDARD_FIND_MODULE_USE_IMPORTED_TARGET OR STANDARD_FIND_MODULE_USE_IMPORTED_TARGET_${_name})
            set(${_name}_LIBRARIES ${_OPT_${_NAME}_TARGET})
          endif()
        endif()

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

      set(_${_name}_FIND_QUIETLY ${${_name}_FIND_QUIETLY})
      set(${_name}_FIND_QUIETLY ${_OPT_${_NAME}_QUIET})
      find_package_handle_standard_args(${_name} DEFAULT_MSG ${_name}_LIBRARIES)
      set(${_name}_FIND_QUIETLY ${_${_name}_FIND_QUIETLY})

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
    extract_version(${_name})
  endif()


  # Print some debug output if either STANDARD_FIND_MODULE_DEBUG
  # or STANDARD_FIND_MODULE_DEBUG_${_name} is set to TRUE
  if(STANDARD_FIND_MODULE_DEBUG OR STANDARD_FIND_MODULE_DEBUG_${_name})
    message(STATUS "${_name}_FOUND = ${${_name}_FOUND}")
    message(STATUS "${_name}_INCLUDE_DIRS = ${${_name}_INCLUDE_DIRS}")
    message(STATUS "${_name}_LIBRARIES = ${${_name}_LIBRARIES}")
    message(STATUS "${_name}_DEFINITIONS = ${${_name}_DEFINITIONS}")
    message(STATUS "${_name}_VERSION = ${${_name}_VERSION}")
    message(STATUS "${_name}_MAJOR_VERSION = ${${_name}_MAJOR_VERSION}")
    message(STATUS "${_name}_MINOR_VERSION = ${${_name}_MINOR_VERSION}")
    message(STATUS "${_name}_PATCH_VERSION = ${${_name}_PATCH_VERSION}")
    message(STATUS "${_name}_TWEAK_VERSION = ${${_name}_TWEAK_VERSION}")
    message(STATUS "${_name}_VERSION_COUNT = ${${_name}_VERSION_COUNT}")
    message(STATUS "${_name}_TARGET = ${_OPT_${_NAME}_TARGET}")
  endif()

endmacro()
