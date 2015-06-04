#.rst:
# FindLibOVR
# -----------
#
# Try to find the LibOVR library.
# Once done this will define the following variables::
#
#  LibOVR_FOUND                   - System has LibOVR
#  LibOVR_VERSION                 - LibOVR version
#  LibOVR_VERSION_PRODUCT         - LibOVR product version
#  LibOVR_VERSION_MAJOR           - LibOVR major version
#  LibOVR_VERSION_MINOR           - LibOVR minor version
#  LibOVR_VERSION_PATCH           - LibOVR patch version
#  LibOVR_VERSION_BUILD           - LibOVR build number
#  LibOVR_VERSION_STRING          - LibOVR version
#  LibOVR_VERSION_DETAILED_STRING - LibOVR version (including build number)
#
# If the LibOVR library was found, the following targets will be
# imported::
#
#  LibOVR::OVRKernel
#  LibOVR::OVR

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
set(LibOVR_OVRKernel_DIR "${OculusSDK_DIR}/LibOVRKernel")
set(LibOVR_OVR_DIR "${OculusSDK_DIR}/LibOVR")

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
      set(_arch "Windows/VS2010")
    elseif(MSVC12)
      set(_arch "Windows/VS2010")
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

set(LibOVR_OVR_Version_h "${LibOVR_OVR_DIR}/Include/OVR_Version.h")
if(EXISTS "${LibOVR_OVR_Version_h}")
  file(STRINGS "${LibOVR_OVR_Version_h}" LibOVR_OVR_Version_h_CONTENTS
       REGEX "#[\t ]*define[ \t]+OVR_((PRODUCT|MAJOR|MINOR|PATCH)_VERSION)|BUILD_NUMBER[ \t]+[0-9]+")

  foreach(_part PRODUCT MAJOR MINOR PATCH)
    string(REGEX REPLACE ".*#[\t ]*define[ \t]+OVR_${_part}_VERSION[ \t]+([0-9]+).*" "\\1"
           LibOVR_VERSION_${_part} "${LibOVR_OVR_Version_h_CONTENTS}")
  endforeach()
  string(REGEX REPLACE ".*#[\t ]*define[ \t]+OVR_BUILD_NUMBER[ \t]+([0-9]+).*" "\\1"
         LibOVR_VERSION_BUILD   "${LibOVR_OVR_Version_h_CONTENTS}")

  set(LibOVR_VERSION "${LibOVR_VERSION_PRODUCT}.${LibOVR_VERSION_MAJOR}.${LibOVR_VERSION_MINOR}.${LibOVR_VERSION_PATCH}")
  set(LibOVR_VERSION_STRING "${LibOVR_VERSION}")
  set(LibOVR_VERSION_DETAILED_STRING "${LibOVR_VERSION}.${LibOVR_VERSION_BUILD}")

  unset(LibOVR_OVR_Version_h_CONTENTS)
endif()
unset(LibOVR_OVR_Version_h)

# Create imported target LibOVR::OVRKernel
add_library(LibOVR::OVRKernel STATIC IMPORTED)
set_target_properties(LibOVR::OVRKernel PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${LibOVR_OVRKernel_DIR}/Src;${LibOVR_OVRKernel_DIR}/Src/Kernel"
)

# Create imported target LibOVR::OVR
add_library(LibOVR::OVR STATIC IMPORTED)
set_target_properties(LibOVR::OVR PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${LibOVR_OVR_DIR}/Include;${LibOVR_OVR_DIR}/Include/Extras"
  INTERFACE_LINK_LIBRARIES "LibOVR::OVRKernel"
)

if(EXISTS "${LibOVR_OVRKernel_DIR}/Lib/${_os}/Debug/${_arch}/${_pref}OVRKernel.${_ext}")
  # Import target "LibOVR::OVRKernel" for configuration "Debug"
  set_property(TARGET LibOVR::OVRKernel APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
  set_target_properties(LibOVR::OVRKernel PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
    IMPORTED_LOCATION_DEBUG "${LibOVR_OVRKernel_DIR}/Lib/${_os}/Debug/${_arch}/${_pref}OVRKernel.${_ext}"
    )

endif()

if(EXISTS "${LibOVR_OVRKernel_DIR}/Lib/${_os}/Release/${_arch}/${_pref}OVRKernel.${_ext}")
  # Import target "LibOVR::OVRKernel" for configuration "Release"
  set_property(TARGET LibOVR::OVRKernel APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
  set_target_properties(LibOVR::OVRKernel PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
    IMPORTED_LOCATION_RELEASE "${LibOVR_OVRKernel_DIR}/Lib/${_os}/Release/${_arch}/${_pref}OVRKernel.${_ext}"
    )
endif()

if(EXISTS "${LibOVR_OVR_DIR}/Lib/${_os}/Debug/${_arch}/${_pref}OVR.${_ext}")
  # Import target "LibOVR::OVR" for configuration "Debug"
  set_property(TARGET LibOVR::OVR APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
  set_target_properties(LibOVR::OVR PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
    IMPORTED_LOCATION_DEBUG "${LibOVR_OVR_DIR}/Lib/${_os}/Debug/${_arch}/${_pref}OVR.${_ext}"
    )
endif()

if(EXISTS "${LibOVR_OVR_DIR}/Lib/${_os}/Release/${_arch}/${_pref}OVR.${_ext}")
  # Import target "LibOVR::OVR" for configuration "Release"
  set_property(TARGET LibOVR::OVR APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
  set_target_properties(LibOVR::OVR PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
    IMPORTED_LOCATION_RELEASE "${LibOVR_OVR_DIR}/Lib/${_os}/Release/${_arch}/${_pref}OVR.${_ext}"
    )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibOVR
                                  FOUND_VAR LibOVR_FOUND
                                  REQUIRED_VARS LibOVR_VERSION_STRING
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
