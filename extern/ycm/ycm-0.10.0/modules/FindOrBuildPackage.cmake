#.rst:
# FindOrBuildPackage
# ------------------
#
# Searches for a package and builds it if it cannot be found.
#
#
# .. command:: find_or_build_package
#
# Searches for a package and builds it if it cannot be found::
#
#  find_or_build_package(<PackageName> <find_package_args>)
#
# This module tries to locate a package using :command:`find_package`.
# If the package cannot be found it tries to build it, by including
# ``Build<package>``. This file should contain the instructions to
# download and build the package (for example using
# :module:`ExternalProject`)
#
# The arguments passed to the function, are passed to the
# :command:`find_package` command.
#
# FIXME Figure out how to handle the REQUIRED and QUIET arguments
#
# This module checks if
# :variable:`CMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY` and/or
# :variable:`CMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY` are, set,
# and eventually explicitly passes ``NO_CMAKE_PACKAGE_REGISTRY`` and/or
# ``NO_CMAKE_SYSTEM_PACKAGE_REGISTRY`` to the :command:`find_package`
# command. These variables will be automatically considered starting
# form CMake 3.1, see also :cmake-issue:`14849`
#
# If the package was found, the ``USE_SYSTEM_<PackageName>`` cached
# variable can be disabled in order to force CMake to build the package
# instead of using the one found on the system.
#
# This function sets these variables::
#
#  HAVE_SYSTEM_<PackageName> # The package was found on the system
#  HAVE_<PackageName> # The package is available
#
# The user should use the latest, to check if a package is available,
# instead of checking using ``<PackageName>_FOUND``. For example
#
# FIXME Check uppercase (${_PKG}) and lowercase (${_pkg}) variables
#
#  .. code-block:: cmake
#
#    find_or_build_package(Foo)
#    if(HAVE_Foo)
#      ...
#    endif()
#
#
# .. variable:: CMAKE_DISABLE_BUILD_PACKAGE_<PackageName>
#
# Using this variable, building a package is explicitly forbidden.
# therefore if the package cannot be found on the system, the
# ``HAVE_<PackageName>`` will be set to false.

#=============================================================================
# Copyright 2013-2014 Istituto Italiano di Tecnologia (IIT)
#   Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of YCM, substitute the full
#  License text for the above reference.)


if(DEFINED __FIND_OR_BUILD_PACKAGE_INCLUDED)
  return()
endif()
set(__FIND_OR_BUILD_PACKAGE_INCLUDED TRUE)


include(CMakeParseArguments)
include(CMakeDependentOption)
include(ExtractVersion)

function(FIND_OR_BUILD_PACKAGE _pkg)
    string(TOUPPER "${_pkg}" _PKG)
    string(REGEX REPLACE "[^A-Z0-9]" "_" _PKG "${_PKG}")

    # Extract version argument from ARGN (if available)
    if(${ARGC} GREATER 1)
        list(GET ARGN 0 _version)
        # Check all possible find_package arguments
        foreach(_arg in EXACT
                        QUIET
                        MODULE
                        REQUIRED
                        COMPONENTS
                        OPTIONAL_COMPONENTS
                        CONFIG
                        NO_MODULE
                        NO_POLICY_SCOPE
                        NAMES
                        CONFIGS
                        HINTS
                        PATHS
                        PATH_SUFFIXES
                        NO_DEFAULT_PATH
                        NO_CMAKE_ENVIRONMENT_PATH
                        NO_CMAKE_PATH
                        NO_SYSTEM_ENVIRONMENT_PATH
                        NO_CMAKE_PACKAGE_REGISTRY
                        NO_CMAKE_BUILDS_PATH
                        NO_CMAKE_SYSTEM_PATH
                        NO_CMAKE_SYSTEM_PACKAGE_REGISTRY
                        CMAKE_FIND_ROOT_PATH_BOTH
                        ONLY_CMAKE_FIND_ROOT_PATH
                        NO_CMAKE_FIND_ROOT_PATH)
            if("${_version}" STREQUAL "${_arg}")
                unset(_version)
                break()
            endif()
        endforeach()
        if(DEFINED _version)
            list(REMOVE_AT ARGN 0)
        endif()
    endif()

    if(${HAVE_${_PKG}})
        return()
    endif()

# Check arguments
    set(_options REQUIRED
                 QUIET
                 MODULE
                 CONFIG
                 NO_MODULE
                 NO_CMAKE_PACKAGE_REGISTRY
                 NO_CMAKE_SYSTEM_PACKAGE_REGISTRY)
    set(_oneValueArgs )
    set(_multiValueArgs COMPONENTS)

    cmake_parse_arguments(_${_PKG} "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" "${ARGN}")

# Arguments for the find_package command
# REQUIRED and QUIET will be set when necessary
    if(DEFINED _${_PKG}_COMPONENTS)
        set(_findArgs COMPONENTS ${_${_PKG}_COMPONENTS} ${_${_PKG}_UNPARSED_ARGUMENTS})
    else()
        set(_findArgs ${_${_PKG}_UNPARSED_ARGUMENTS})
    endif()

    # Disable package cache when not done automatically by CMake
    # This is a workaround for CMake bug #14849
    unset(_find_or_build_package_registryArgs)
    if(NOT CMAKE_REQUIRED_VERSION VERSION_LESS 3.1)
        message(AUTHOR_WARNING "Disabling cmake cache is supported since CMake 3.1. You can remove this check")
    endif()
    if(CMAKE_VERSION VERSION_LESS 3.1)
        if(CMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY OR _${_PKG}_NO_CMAKE_PACKAGE_REGISTRY)
            list(APPEND _find_or_build_package_registryArgs NO_CMAKE_PACKAGE_REGISTRY)
        endif()
        if(CMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY OR _${_PKG}_NO_CMAKE_SYSTEM_PACKAGE_REGISTRY)
            list(APPEND _find_or_build_package_registryArgs NO_CMAKE_SYSTEM_PACKAGE_REGISTRY)
        endif()
    endif()


# Preliminary find_package to enable/disable USE_SYSTEM_${_PKG} option
    # Use the FindPkg.cmake module first
    if(NOT _${_PKG}_NO_MODULE AND NOT _${_PKG}_CONFIG)
        # FIXME This might require to check for all the other arguments, or they
        #       might conflict with the MODULE argument
        find_package(${_pkg} ${_version} ${_findArgs} MODULE QUIET)
    endif()

    # If the module failed, search a PkgConfig.cmake file
    if(NOT ${_pkg}_FOUND AND NOT ${_PKG}_FOUND AND NOT _${_PKG}_MODULE)
        find_package(${_pkg} ${_version} ${_findArgs} ${_find_or_build_package_registryArgs} CONFIG QUIET)
    endif()

    if(${_pkg}_FOUND OR ${_PKG}_FOUND)
        set(HAVE_SYSTEM_${_PKG} 1)
        get_property(_ycm_projects GLOBAL PROPERTY YCM_PROJECTS)
        list(APPEND _ycm_projects ${_pkg})
        list(REMOVE_DUPLICATES _ycm_projects)
        set_property(GLOBAL PROPERTY YCM_PROJECTS ${_ycm_projects})
    endif()
    cmake_dependent_option(USE_SYSTEM_${_PKG} "Use system installed ${_pkg}" ON "HAVE_SYSTEM_${_PKG}" OFF)
    mark_as_advanced(USE_SYSTEM_${_PKG})


    if(USE_SYSTEM_${_PKG})
        # The system version was found and should be used
        set(HAVE_${_PKG} 1)
    elseif(CMAKE_DISABLE_BUILD_PACKAGE_${_PKG})
        # Building the package was explicitly disabled
        set(HAVE_${_PKG} 0)
    else()
        # The system version was not found or the user chose not to use it,
        # building the package was not disabled, therefore we need to build it.

        # Save variables to restore them at the end
        foreach(_var REQUIRED
                     QUIETLY
                     VERSION
                     VERSION_MAJOR
                     VERSION_MINOR
                     VERSION_PATCH
                     VERSION_TWEAK
                     VERSION_COUNT
                     COMPONENTS)
            unset(_${_PKG}_BUILD_${_var})
            if(DEFINED ${_PKG}_BUILD_${_var})
                set(_${_PKG}_BUILD_${_var} ${_PKG}_BUILD_${_var})
                unset(${_PKG}_BUILD_${_var})
            endif()
        endforeach()

        set(${_PKG}_BUILD_REQUIRED ${_${_PKG}_REQUIRED})
        set(${_PKG}_BUILD_QUIETLY ${_${_PKG}_QUIET})
        if(DEFINED ${_${_PKG}_VERSION})
            set(${_PKG}_BUILD_VERSION ${_${_PKG}_VERSION})
            extract_version(${_PKG}_BUILD REVERSE_NAME)
        endif()
        set(${_PKG}_BUILD_COMPONENTS ${_${_PKG}_COMPONENTS})

        if(TARGET ${_pkg})
            # Weird. We found the recipe, but this doesn't add a TARGET ${_pkg}
            # Should not happen, let's print an AUTHOR_WARNING
            message(AUTHOR_WARNING "A ${_pkg} target already exists before including Build${_pkg}.")
        endif()
        # Include the Build recipe
        include(Build${_pkg} OPTIONAL RESULT_VARIABLE _${_PKG}_RECIPE)
        if(NOT _${_PKG}_RECIPE)
            # We couldn't find the build recipe
            set(HAVE_${_PKG} 0)
        else()
            if(NOT TARGET ${_pkg})
                # Weird. We found the recipe, but this doesn't add a TARGET ${_pkg}
                # Should not happen, let's print an AUTHOR_WARNING
                message(AUTHOR_WARNING "The file ${_${_PKG}_RECIPE} does not define a ${_pkg} target.")
            endif()
            set(HAVE_${_PKG} 1)
        endif()

        # Restore original value for the variables
        foreach(_var REQUIRED
                     QUIETLY
                     VERSION
                     VERSION_MAJOR
                     VERSION_MINOR
                     VERSION_PATCH
                     VERSION_TWEAK
                     VERSION_COUNT
                     COMPONENTS)
            if(DEFINED _${_PKG}_BUILD_${_var})
                set(${_PKG}_BUILD_${_var} _${_PKG}_BUILD_${_var})
            else()
                unset(${_PKG}_BUILD_${_var})
            endif()
            unset(_${_PKG}_BUILD_${_var})
        endforeach()
    endif()

    # Display errors/messages
    set(_runFind 0)
    if(NOT HAVE_${_PKG})
        if(${_${_PKG}_REQUIRED})
            message(SEND_ERROR "Cannot find or build package ${_pkg}")
            set(_runFind 1)
        elseif(NOT ${_${_PKG}_QUIET})
            message(STATUS "Cannot find or build package ${_pkg}")
            set(_runFind 1)
        endif()
    elseif(NOT ${_${_PKG}_QUIET})
        if(USE_SYSTEM_${_PKG})
            set(_runFind 1)
        else()
            message(STATUS "Package ${_pkg} not found. Will be downloaded and built.")
        endif()
    endif()

    if(_runFind)
        # Rerun find_package with all the arguments to display output
        find_package(${_pkg} ${_version} ${ARGN} ${_find_or_build_package_registryArgs})
    endif()

    get_cmake_property(_vars VARIABLES)
    foreach(_var ${_vars})
        if(_var MATCHES "^HAVE_")
            set(${_var} ${${_var}} PARENT_SCOPE)
        endif()
    endforeach()
endfunction()
