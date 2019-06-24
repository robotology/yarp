#.rst:
# FindACE
# -------
#
# Try to find the ACE library
#
# Targets set::
#
#   ACE::ACE (links the ACE library)
#   ACE::ACE_INLINE (INTERFACE library for ACE inlines only)
#
# Variables set::
#
#   ACE_FOUND           - System has ACE library
#   ACE_LIBRARIES       - ACE link libraries
#   ACE_INCLUDE_DIRS    - ACE library include directories
#   ACE_DEFINITIONS     - Additional compiler flags for ACE library
#   ACE_VERSION         - ACE library version
#   ACE_MAJOR_VERSION   - ACE major version
#   ACE_MINOR_VERSION   - ACE minor version
#   ACE_BETA_VERSION    - ACE beta version
#
# Options variables::
#
#   ACE_INLINE (default ON)
#     The ACE::ACE target passes __ACE_INLINE__ by default, this can be
#     changed by setting ACE_INLINE = OFF.
#     The ACE_DEFINITIONS variable is also influenced by the same option.

#=============================================================================
# Copyright 2009 RobotCub Consortium
# Copyright 2013-2014 Istituto Italiano di Tecnologia (IIT)
#   Authors: Alexandre Bernardino <alex@isr.ist.utl.pt>
#            Paul Fitzpatrick <paulfitz@alum.mit.edu>
#            Lorenzo Natale <lorenzo.natale@iit.it>
#            Daniele E. Domenichelli <daniele.domenichelli@iit.it>
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


include(StandardFindModule)
standard_find_module(ACE ACE SKIP_CMAKE_CONFIG NOT_REQUIRED)

########################################################################
## Find include directory
find_path(ACE_INCLUDE_DIR
          NAMES ace/ACE.h
          PATHS $ENV{ACE_ROOT}
                $ENV{ACE_ROOT}/include
                /usr/include
                /usr/local/include
          DOC "directory containing ace/ACE.h for ACE library")
mark_as_advanced(ACE_INCLUDE_DIR)

set(ACE_INCLUDE_DIRS ${ACE_INCLUDE_DIR})

########################################################################
## Find libraries
find_library(ACE_ACE_LIBRARY_RELEASE
             NAMES ACE
                   ace
             PATHS $ENV{ACE_ROOT}/lib
                   $ENV{ACE_ROOT}
                   /usr/lib
                   /usr/local/lib
             DOC "ACE library file")

find_library(ACE_ACE_LIBRARY_DEBUG
             NAMES ACEd
                   aced
             PATHS $ENV{ACE_ROOT}/lib
                   $ENV{ACE_ROOT}
                   /usr/lib
                   /usr/local/lib
             DOC "ACE library file (debug version)")

include(SelectLibraryConfigurations)
select_library_configurations(ACE_ACE)

set(ACE_LIBRARIES ${ACE_ACE_LIBRARY})

########################################################################
## Definitions

if (NOT DEFINED ACE_INLINE OR ACE_INLINE)
  set(ACE_DEFINITIONS "__ACE_INLINE__")
endif()

########################################################################
## OS-specific extra linkage

# Solaris needs some extra libraries that may not have been found already
if(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
  set(ACE_EXTRA_LIBRARIES socket rt nsl)
endif(CMAKE_SYSTEM_NAME STREQUAL "SunOS")

# ACE package doesn't specify that pthread and rt are needed, which is
# a problem for users of GoLD.  Link pthread (just on Linux for now).
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(ACE_EXTRA_LIBRARIES pthread rt)
endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

# Windows needs some extra libraries
if(WIN32 AND NOT CYGWIN)
  set(ACE_EXTRA_LIBRARIES winmm)
endif(WIN32 AND NOT CYGWIN)

# Mingw needs some extra libraries
if(MINGW)
  set(ACE_EXTRA_LIBRARIES winmm ws2_32 wsock32)
endif(MINGW)

list(APPEND ACE_LIBRARIES ${ACE_EXTRA_LIBRARIES})

########################################################################
## Set up flags and complain to user if necessary
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ACE FOUND_VAR ACE_FOUND
                                      REQUIRED_VARS ACE_LIBRARIES ACE_INCLUDE_DIRS
                                      VERSION_VAR ACE_VERSION)

########################################################################
# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
  set_package_properties(ACE PROPERTIES DESCRIPTION "The ADAPTIVE Communication Environment"
                                        URL "http://www.cs.wustl.edu/~schmidt/ACE.html")
endif()

########################################################################
# Stop here if ACE was not found
if(NOT ACE_FOUND)
  return()
endif()

########################################################################
## Read version from ace/Version.h file
if(EXISTS "${ACE_INCLUDE_DIR}/ace/Version.h")
  file(STRINGS "${ACE_INCLUDE_DIR}/ace/Version.h" _contents REGEX "#define ACE_[A-Z]+_VERSION[ \t]+")

  if(_contents)
    string(REGEX REPLACE ".*#define ACE_MAJOR_VERSION[ \t]+([0-9]+).*" "\\1" ACE_MAJOR_VERSION "${_contents}")
    string(REGEX REPLACE ".*#define ACE_MINOR_VERSION[ \t]+([0-9]+).*" "\\1" ACE_MINOR_VERSION "${_contents}")
    string(REGEX REPLACE ".*#define ACE_BETA_VERSION[ \t]+([0-9]+).*" "\\1" ACE_BETA_VERSION "${_contents}")

    if(NOT ACE_MAJOR_VERSION MATCHES "[0-9]+")
      message(FATAL_ERROR "Version parsing failed for ACE_MAJOR_VERSION!")
    endif()
    if(NOT ACE_MINOR_VERSION MATCHES "[0-9]+")
      message(FATAL_ERROR "Version parsing failed for ACE_MINOR_VERSION!")
    endif()
    if(NOT ACE_BETA_VERSION MATCHES "[0-9]+")
    message(FATAL_ERROR "Version parsing failed for ACE_BETA_VERSION!")
    endif()
  endif()
  set(ACE_VERSION "${ACE_MAJOR_VERSION}.${ACE_MINOR_VERSION}.${ACE_BETA_VERSION}")
endif()

########################################################################
## If ACE was found, check if some features are available

# Create and populate the ACE::ACE target
add_library(ACE::ACE UNKNOWN IMPORTED)

set_target_properties(ACE::ACE PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${ACE_INCLUDE_DIRS}")
set_target_properties(ACE::ACE PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${ACE_INCLUDE_DIRS}")
set_target_properties(ACE::ACE PROPERTIES INTERFACE_LINK_LIBRARIES "${ACE_EXTRA_LIBRARIES}")
if (NOT DEFINED ACE_INLINE OR ACE_INLINE)
  set_target_properties(ACE::ACE PROPERTIES INTERFACE_COMPILE_DEFINITIONS "__ACE_INLINE__")
endif()

if(ACE_ACE_LIBRARY_RELEASE)
  set_property(TARGET ACE::ACE APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
  set_target_properties(ACE::ACE PROPERTIES IMPORTED_LOCATION_RELEASE "${ACE_ACE_LIBRARY_RELEASE}")
  set_target_properties(ACE::ACE PROPERTIES IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX")
endif()

if(ACE_ACE_LIBRARY_DEBUG)
  set_property(TARGET ACE::ACE APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
  set_target_properties(ACE::ACE PROPERTIES IMPORTED_LOCATION_DEBUG "${ACE_ACE_LIBRARY_DEBUG}")
  set_target_properties(ACE::ACE PROPERTIES IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX")
endif()

# Create and populate the ACE::ACE_INLINE target
add_library(ACE::ACE_INLINE INTERFACE IMPORTED)

set_target_properties(ACE::ACE_INLINE PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${ACE_INCLUDE_DIRS}")
set_target_properties(ACE::ACE_INLINE PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${ACE_INCLUDE_DIRS}")
set_target_properties(ACE::ACE_INLINE PROPERTIES INTERFACE_LINK_LIBRARIES "${ACE_EXTRA_LIBRARIES}")
set_target_properties(ACE::ACE_INLINE PROPERTIES INTERFACE_COMPILE_DEFINITIONS "__ACE_INLINE__")


########################################################################
## Compatibility with older versions

if (NOT COMMAND _FindACE_Deprecated)
  # If set, save variables for later
  set(_CMAKE_TRY_COMPILE_CONFIGURATION ${CMAKE_TRY_COMPILE_CONFIGURATION})
  set(_CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES})
  set(_CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})

  include (CheckCXXSourceCompiles)

  # "__ACE_INLINE__" is needed in some configurations
  set(_ACE_NEEDS_INLINE_CPP "
  #include <ace/OS_NS_unistd.h>
  #include <ace/Time_Value.h>
  void time_delay(double seconds) {
      ACE_Time_Value tv;
      tv.sec (long(seconds));
      tv.usec (long((seconds-long(seconds)) * 1.0e6));
      ACE_OS::sleep(tv);
  }
  int main(int argc, char *argv[]) {
      time_delay(1);
      return 0;
  }
  ")
  set(CMAKE_REQUIRED_INCLUDES ${ACE_INCLUDE_DIRS})
  set(CMAKE_REQUIRED_LIBRARIES ${ACE_LIBRARIES})
  if(ACE_ACE_LIBRARY_RELEASE)
    set(CMAKE_TRY_COMPILE_CONFIGURATION "Release")
    check_cxx_source_compiles("${_ACE_NEEDS_INLINE_CPP}" ACE_COMPILES_WITHOUT_INLINE_RELEASE)
  endif()
  if(ACE_ACE_LIBRARY_DEBUG)
    set(CMAKE_TRY_COMPILE_CONFIGURATION "Debug")
    check_cxx_source_compiles("${_ACE_NEEDS_INLINE_CPP}" ACE_COMPILES_WITHOUT_INLINE_DEBUG)
  endif()

  # Reset variables to their original values
  set(CMAKE_TRY_COMPILE_CONFIGURATION ${_CMAKE_TRY_COMPILE_CONFIGURATION})
  set(CMAKE_REQUIRED_INCLUDES ${_CMAKE_REQUIRED_INCLUDES})
  set(CMAKE_REQUIRED_LIBRARIES ${_CMAKE_REQUIRED_LIBRARIES})
  unset(_CMAKE_TRY_COMPILE_CONFIGURATION)
  unset(_CMAKE_REQUIRED_INCLUDES)
  unset(_CMAKE_REQUIRED_LIBRARIES)

  function(_FindACE_Deprecated _variable _access _value _current_list_file _stack)
    message(DEPRECATION "${_variable} is deprecated")
  endfunction()

  set(ACE_LIBRARY_RELEASE ${ACE_ACE_LIBRARY_RELEASE})
  set(ACE_LIBRARY_DEBUG ${ACE_ACE_LIBRARY_DEBUG})
  set(ACE_LIBRARY ${ACE_ACE_LIBRARY})
  set(ACE_HAS_STRING_HASH 1)

  # Check for ACE_INET_Addr::is_loopback
  if("${ACE_VERSION}" VERSION_LESS "5.4.8")
    set(ACE_ADDR_HAS_LOOPBACK_METHOD 0)
  else()
    set(ACE_ADDR_HAS_LOOPBACK_METHOD 1)
  endif()

  variable_watch(ACE_LIBRARY_RELEASE _FindACE_Deprecated)
  variable_watch(ACE_LIBRARY_DEBUG _FindACE_Deprecated)
  variable_watch(ACE_LIBRARY _FindACE_Deprecated)
  variable_watch(ACE_HAS_STRING_HASH _FindACE_Deprecated)
  variable_watch(ACE_ADDR_HAS_LOOPBACK_METHOD _FindACE_Deprecated)
  variable_watch(ACE_COMPILES_WITHOUT_INLINE_RELEASE _FindACE_Deprecated)
  variable_watch(ACE_COMPILES_WITHOUT_INLINE_DEBUG _FindACE_Deprecated)
endif()
