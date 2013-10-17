# Try to find the ACE library
#
# Variables set:
#   ACE_FOUND
#   ACE_LIBRARIES
#   ACE_INCLUDE_DIRS
#   ACE_VERSION
#   ACE_MAJOR_VERSION
#   ACE_MINOR_VERSION
#   ACE_BETA_VERSION

# Copyright: (C) 2009 RobotCub Consortium
# Copyright: (C) 2013  iCub Facility, Istituto Italiano di Tecnologia
# Authors: Alexandre Bernardino, Paul Fitzpatrick, Lorenzo Natale, Daniele E. Domenichelli
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


include(MacroStandardFindModule)
macro_standard_find_module(ACE ACE SKIP_CMAKE_CONFIG NOT_REQUIRED)


if(ACE_FOUND)
    set(ACE_BETA_VERSION ${ACE_PATCH_VERSION})
    unset(ACE_PATCH_VERSION)
    unset(ACE_TWEAK_VERSION)
else()
    ########################################################################
    ##  general find

    find_path(ACE_INCLUDE_DIR
              NAMES ace/ACE.h
              PATHS $ENV{ACE_ROOT}
                    $ENV{ACE_ROOT}/include
                    /usr/include
                    /usr/local/include
              DOC "directory containing ace/*.h for ACE library")
    mark_as_advanced(ACE_INCLUDE_DIR)

    find_library(ACE_ACE_LIBRARY_RELEASE
                 NAMES ACE
                       ace
                 PATHS $ENV{ACE_ROOT}/lib
                       $ENV{ACE_ROOT}
                       /usr/lib
                       /usr/local/lib
                 DOC "ACE library file")

    if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
        set(CMAKE_DEBUG_POSTFIX "d")
        set(_CMAKE_DEBUG_POSTFIX_DEFINED 1)
    endif()

    find_library(ACE_ACE_LIBRARY_DEBUG
                 NAMES ACE${CMAKE_DEBUG_POSTFIX}
                       ace${CMAKE_DEBUG_POSTFIX}
                 PATHS $ENV{ACE_ROOT}/lib
                       $ENV{ACE_ROOT}
                       /usr/lib
                       /usr/local/lib
                 DOC "ACE library file (debug version)")

    if(DEFINED _CMAKE_DEBUG_POSTFIX_DEFINED)
        unset(CMAKE_DEBUG_POSTFIX)
        unset(_CMAKE_DEBUG_POSTFIX_DEFINED)
    endif()

    include(SelectLibraryConfigurations)
    select_library_configurations(ACE_ACE)

    set(ACE_LIBRARIES ${ACE_ACE_LIBRARY})
    set(ACE_INCLUDE_DIRS ${ACE_INCLUDE_DIR})

    ########################################################################
    ## OS-specific extra linkage

    # Solaris needs some extra libraries that may not have been found already
    if(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
        list(APPEND ACE_LIBRARIES socket rt nsl)
    endif(CMAKE_SYSTEM_NAME STREQUAL "SunOS")

    # ACE package doesn't specify that pthread and rt are needed, which is
    # a problem for users of GoLD.  Link pthread (just on Linux for now).
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        list(APPEND ACE_LIBRARIES pthread rt)
    endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

    # Windows needs some extra libraries
    if(WIN32 AND NOT CYGWIN)
        list(APPEND ACE_LIBRARIES winmm)
    endif(WIN32 AND NOT CYGWIN)

    # Mingw needs some extra libraries
    if(MINGW)
        list(APPEND ACE_LIBRARIES winmm ws2_32 wsock32)
    endif(MINGW)


    ########################################################################
    ## finished - now just set up flags and complain to user if necessary
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(ACE FOUND_VAR ACE_FOUND
                                          REQUIRED_VARS ACE_LIBRARIES ACE_INCLUDE_DIRS
                                          VERSION_VAR ACE_VERSION)

    ########################################################################
    ## Read version from ace/Version.h file
    if(ACE_FOUND)
        file(STRINGS ${ACE_INCLUDE_DIR}/ace/Version.h _contents REGEX "#define ACE_[A-Z]+_VERSION[ \t]+")
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
        else()
            message(FATAL_ERROR "Include file ace/Version.h does not exist")
        endif()

        set(ACE_VERSION "${ACE_MAJOR_VERSION}.${ACE_MINOR_VERSION}.${ACE_BETA_VERSION}")
    endif()

endif()

# Compatibility
set(ACE_LIBRARY_RELEASE ${ACE_ACE_LIBRARY_RELEASE})
set(ACE_LIBRARY_DEBUG ${ACE_ACE_LIBRARY_DEBUG})
set(ACE_LIBRARY ${ACE_ACE_LIBRARY})

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(ACE PROPERTIES DESCRIPTION "The ADAPTIVE Communication Environment"
                                          URL "http://www.cs.wustl.edu/~schmidt/ACE.html")
endif()
