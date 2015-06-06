#.rst:
# FindLibOVR
# -----------
#
# Find the LibOVR library in Oculus Rift SDK.
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the following :prop_tgt:`IMPORTED` targets if
# LibOVR has been found::
#
#   LibOVR::OVRKernel
#   LibOVR::OVR
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables::
#
#   LibOVR_FOUND                   - System has LibOVR
#   LibOVR_VERSION                 - LibOVR version
#   LibOVR_VERSION_PRODUCT         - LibOVR product version
#   LibOVR_VERSION_MAJOR           - LibOVR major version
#   LibOVR_VERSION_MINOR           - LibOVR minor version
#   LibOVR_VERSION_PATCH           - LibOVR patch version
#   LibOVR_VERSION_BUILD           - LibOVR build number
#   LibOVR_VERSION_STRING          - LibOVR version
#   LibOVR_VERSION_DETAILED_STRING - LibOVR version (including build number)
#   LibOVR_INCLUDE_DIRS            - Include directories for LibOVR
#   LibOVR_LIBRARIES               - libraries to link against LibOVR

#=============================================================================
# Copyright 2015  iCub Facility, Istituto Italiano di Tecnologia
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



set(OculusSDK_DIR "$ENV{OculusSDK_ROOT}")

unset(_os)
unset(_arch)
unset(_ext)
unset(_pref)
if(WIN32)
  if("${CMAKE_GENERATOR}" MATCHES "^Visual Studio")
    if(${CMAKE_GENERATOR_PLATFORM} MATCHES "x64" OR "${CMAKE_GENERATOR}" MATCHES "Win64")
      set(_os x64)
    else()
      set(_os Win32)
    endif()
    if(MSVC10)
      set(_arch "Windows/VS2010")
    elseif(MSVC11)
      set(_arch "Windows/VS2012")
    elseif(MSVC12)
      set(_arch "Windows/VS2013")
    endif()
  endif()
  set(_ext lib)
  set(_pref Lib)
elseif(UNIX AND NOT APPLE)
  set(_os "Linux/${CMAKE_SYSTEM_PROCESSOR}")
  set(_arch "")
  set(_ext a)
  set(_pref lib)
endif()

if(NOT DEFINED _os OR NOT DEFINED _arch OR NOT DEFINED _ext OR NOT DEFINED _pref)
  set(LibOVR_FOUND FALSE)
  return()
endif()

find_path(LibOVR_LibOVRKernel_INCLUDE_DIR "Kernel/OVR_Types.h"
          NO_DEFAULT_PATH
          PATHS ENV OculusSDK_ROOT
          PATH_SUFFIXES "LibOVRKernel/Src")
find_path(LibOVR_LibOVRKernel_INCLUDE_DIR "Kernel/OVR_Types.h")
mark_as_advanced(LibOVR_LibOVRKernel_INCLUDE_DIR)

find_path(LibOVR_LibOVR_INCLUDE_DIR "OVR_Version.h"
          NO_DEFAULT_PATH
          PATHS ENV OculusSDK_ROOT
          PATH_SUFFIXES "LibOVR/Include")
find_path(LibOVR_LibOVR_INCLUDE_DIR "OVR_Version.h")
mark_as_advanced(LibOVR_LibOVR_INCLUDE_DIR)

find_path(LibOVR_LibOVR_Extras_INCLUDE_DIR "OVR_Math.h"
          NO_DEFAULT_PATH
          PATHS ENV OculusSDK_ROOT
          PATH_SUFFIXES "LibOVR/Include/Extras")
find_path(LibOVR_LibOVR_Extras_INCLUDE_DIR "OVR_Math.h")
mark_as_advanced(LibOVR_LibOVR_Extras_INCLUDE_DIR)

find_library(LibOVR_LibOVRKernel_LIBRARY_RELEASE
             NAMES OVRKernel
             PATHS ENV OculusSDK_ROOT
             NO_DEFAULT_PATH
             PATH_SUFFIXES "LibOVRKernel/Lib/${_os}/Release/${_arch}")
find_library(LibOVR_LibOVRKernel_LIBRARY_RELEASE
             NAMES OVRKernel)
mark_as_advanced(LibOVR_LibOVRKernel_LIBRARY_RELEASE)

find_library(LibOVR_LibOVRKernel_LIBRARY_DEBUG
             NAMES OVRKernel
             PATHS ENV OculusSDK_ROOT
             NO_DEFAULT_PATH
             PATH_SUFFIXES "LibOVRKernel/Lib/${_os}/Debug/${_arch}")
mark_as_advanced(LibOVR_LibOVRKernel_LIBRARY_DEBUG)

find_library(LibOVR_LibOVR_LIBRARY_RELEASE
             NAMES OVR
             PATHS ENV OculusSDK_ROOT
             NO_DEFAULT_PATH
             PATH_SUFFIXES "LibOVR/Lib/${_os}/Release/${_arch}")
find_library(LibOVR_LibOVR_LIBRARY_RELEASE
             NAMES OVR)
mark_as_advanced(LibOVR_LibOVR_LIBRARY_RELEASE)

find_library(LibOVR_LibOVR_LIBRARY_DEBUG
             NAMES OVR
             PATHS ENV OculusSDK_ROOT
             NO_DEFAULT_PATH
             PATH_SUFFIXES "LibOVR/Lib/${_os}/Debug/${_arch}")
mark_as_advanced(LibOVR_LibOVR_LIBRARY_DEBUG)


include(SelectLibraryConfigurations)
select_library_configurations(LibOVR_LibOVRKernel)
select_library_configurations(LibOVR_LibOVR)


set(LibOVR_INCLUDE_DIRS "${LibOVR_LibOVR_INCLUDE_DIR}"
                        "${LibOVR_LibOVR_Extras_INCLUDE_DIR}"
                        "${LibOVR_LibOVRKernel_INCLUDE_DIR}"
                        "${LibOVR_LibOVRKernel_INCLUDE_DIR}}/Kernel")
set(LibOVR_LIBRARIES "${LibOVR_LibOVR_LIBRARY}"
                     "${LibOVR_LibOVRKernel_LIBRARY}")


if(EXISTS "${LibOVR_LibOVR_INCLUDE_DIR}/OVR_Version.h")
  file(STRINGS "${LibOVR_LibOVR_INCLUDE_DIR}/OVR_Version.h" _contents
       REGEX "#[\t ]*define[ \t]+OVR_((PRODUCT|MAJOR|MINOR|PATCH)_VERSION)|BUILD_NUMBER[ \t]+[0-9]+")

  foreach(_part PRODUCT MAJOR MINOR PATCH)
    string(REGEX REPLACE ".*#[\t ]*define[ \t]+OVR_${_part}_VERSION[ \t]+([0-9]+).*" "\\1"
           LibOVR_VERSION_${_part} "${_contents}")
  endforeach()
  string(REGEX REPLACE ".*#[\t ]*define[ \t]+OVR_BUILD_NUMBER[ \t]+([0-9]+).*" "\\1"
         LibOVR_VERSION_BUILD   "${_contents}")

  set(LibOVR_VERSION "${LibOVR_VERSION_PRODUCT}.${LibOVR_VERSION_MAJOR}.${LibOVR_VERSION_MINOR}.${LibOVR_VERSION_PATCH}")
  set(LibOVR_VERSION_STRING "${LibOVR_VERSION}")
  set(LibOVR_VERSION_DETAILED_STRING "${LibOVR_VERSION}.${LibOVR_VERSION_BUILD}")
endif()

# Create imported target LibOVR::OVRKernel
add_library(LibOVR::OVRKernel STATIC IMPORTED)
set_target_properties(LibOVR::OVRKernel PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${LibOVR_LibOVRKernel_INCLUDE_DIR};${LibOVR_LibOVRKernel_INCLUDE_DIR}/Kernel"
)

# Create imported target LibOVR::OVR
add_library(LibOVR::OVR STATIC IMPORTED)
set_target_properties(LibOVR::OVR PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${LibOVR_LibOVR_INCLUDE_DIR};${LibOVR_LibOVR_Extras_INCLUDE_DIR}"
  INTERFACE_LINK_LIBRARIES "LibOVR::OVRKernel"
)

# Import target "LibOVR::OVRKernel" for configuration "Debug"
if(EXISTS "${LibOVR_LibOVRKernel_LIBRARY_DEBUG}")
  set_property(TARGET LibOVR::OVRKernel APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
  set_target_properties(LibOVR::OVRKernel PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
    IMPORTED_LOCATION_DEBUG "${LibOVR_LibOVRKernel_LIBRARY_DEBUG}"
    )
endif()

# Import target "LibOVR::OVRKernel" for configuration "Release"
if(EXISTS "${LibOVR_LibOVRKernel_LIBRARY_RELEASE}")
  set_property(TARGET LibOVR::OVRKernel APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
  set_target_properties(LibOVR::OVRKernel PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
    IMPORTED_LOCATION_RELEASE "${LibOVR_LibOVRKernel_LIBRARY_RELEASE}"
    )
endif()

# Import target "LibOVR::OVR" for configuration "Debug"
if(EXISTS "${LibOVR_LibOVR_LIBRARY_DEBUG}")
  set_property(TARGET LibOVR::OVR APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
  set_target_properties(LibOVR::OVR PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
    IMPORTED_LOCATION_DEBUG "${LibOVR_LibOVR_LIBRARY_DEBUG}"
    )
endif()

# Import target "LibOVR::OVR" for configuration "Release"
if(EXISTS "${LibOVR_LibOVR_LIBRARY_RELEASE}")
  set_property(TARGET LibOVR::OVR APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
  set_target_properties(LibOVR::OVR PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
    IMPORTED_LOCATION_RELEASE "${LibOVR_LibOVR_LIBRARY_RELEASE}"
    )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibOVR
                                  FOUND_VAR LibOVR_FOUND
                                  REQUIRED_VARS LibOVR_LIBRARIES
                                                LibOVR_INCLUDE_DIRS
                                  VERSION_VAR LibOVR_VERSION_STRING)

if(FindLibOVR_DEBUG)
  include(CMakePrintHelpers)
  cmake_print_variables(LibOVR_FOUND
                        LibOVR_VERSION
                        LibOVR_VERSION_PRODUCT
                        LibOVR_VERSION_MAJOR
                        LibOVR_VERSION_MINOR
                        LibOVR_VERSION_PATCH
                        LibOVR_VERSION_BUILD
                        LibOVR_VERSION_STRING
                        LibOVR_VERSION_DETAILED_STRING)
  cmake_print_properties(TARGETS LibOVR::OVRKernel
                                 LibOVR::OVR
                         PROPERTIES IMPORTED_CONFIGURATIONS
                                    IMPORTED_LOCATION_RELEASE
                                    IMPORTED_LOCATION_DEBUG)
endif()
