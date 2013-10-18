# Try to find the ACE library
#
# Variables set:
#   ACE_FOUND
#   ACE_LIBRARIES
#   ACE_INCLUDE_DIRS
#
#   ACE_VERSION
#   ACE_MAJOR_VERSION
#   ACE_MINOR_VERSION
#   ACE_BETA_VERSION
#
#   ACE_COMPILES_WITHOUT_INLINE_RELEASE
#   ACE_COMPILES_WITHOUT_INLINE_DEBUG
#   ACE_ADDR_HAS_LOOPBACK_METHOD
#   ACE_HAS_STRING_HASH

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


########################################################################
## If ACE was found, check if some features are available

if(ACE_FOUND)

    # If set, save variables for later
    if(DEFINED CMAKE_TRY_COMPILE_CONFIGURATION)
        set(_CMAKE_TRY_COMPILE_CONFIGURATION ${CMAKE_TRY_COMPILE_CONFIGURATION})
    else()
        unset(_CMAKE_TRY_COMPILE_CONFIGURATION)
    endif()

    if(DEFINED CMAKE_REQUIRED_INCLUDES)
        set(_CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES})
    else()
        unset(_CMAKE_TRY_COMPILE_CONFIGURATION)
    endif()
    set(CMAKE_REQUIRED_INCLUDES ${ACE_INCLUDE_DIRS})

    if(DEFINED CMAKE_REQUIRED_LIBRARIES)
        set(_CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
    else()
        unset(_CMAKE_REQUIRED_LIBRARIES)
    endif()
    set(CMAKE_REQUIRED_LIBRARIES ${ACE_LIBRARIES})

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
    if(ACE_ACE_LIBRARY_RELEASE)
        set(CMAKE_TRY_COMPILE_CONFIGURATION "Release")
        check_cxx_source_compiles("${_ACE_NEEDS_INLINE_CPP}" ACE_COMPILES_WITHOUT_INLINE_RELEASE)
    endif()
    if(ACE_ACE_LIBRARY_DEBUG)
        set(CMAKE_TRY_COMPILE_CONFIGURATION "Debug")
        check_cxx_source_compiles("${_ACE_NEEDS_INLINE_CPP}" ACE_COMPILES_WITHOUT_INLINE_DEBUG)
    endif()


    # Check for ACE_INET_Addr::is_loopback
    if("${ACE_VERSION}" VERSION_LESS "5.4.8")
        set(ACE_ADDR_HAS_LOOPBACK_METHOD 0)
    else()
        set(ACE_ADDR_HAS_LOOPBACK_METHOD 1)
    endif()


    # Check if std::string can be used with ACE hash map
    set(_ACE_HAS_STRING_HASH_CPP "
#include <string>
#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>
#include <ace/Functor_String.h>
int main(int argc, char *argv[]) {
    ACE_Hash_Map_Manager<std::string,std::string,ACE_Null_Mutex> my_map;
    ACE_Hash_Map_Entry<std::string,std::string> *it = NULL;
    my_map.find(\"hello\",it);
    return 0;
}
")
    check_cxx_source_compiles("${_ACE_HAS_STRING_HASH_CPP}" ACE_HAS_STRING_HASH)


    # Reset variables to their original values
    if(DEFINED _CMAKE_TRY_COMPILE_CONFIGURATION)
        set(CMAKE_TRY_COMPILE_CONFIGURATION ${_CMAKE_TRY_COMPILE_CONFIGURATION})
        unset(_CMAKE_TRY_COMPILE_CONFIGURATION)
    else()
        unset(CMAKE_TRY_COMPILE_CONFIGURATION)
    endif()

    if(DEFINED _CMAKE_REQUIRED_INCLUDES)
        set(CMAKE_REQUIRED_INCLUDES ${_CMAKE_REQUIRED_INCLUDES})
        unset(${_CMAKE_REQUIRED_INCLUDES})
    else()
        unset(CMAKE_REQUIRED_INCLUDES)
    endif()

    if(DEFINED _CMAKE_REQUIRED_LIBRARIES)
        set(CMAKE_REQUIRED_LIBRARIES ${_CMAKE_REQUIRED_LIBRARIES})
        unset(_CMAKE_REQUIRED_LIBRARIES)
    else()
        unset(CMAKE_REQUIRED_LIBRARIES)
    endif()

endif()


########################################################################
## Compatibility with older versions
set(ACE_LIBRARY_RELEASE ${ACE_ACE_LIBRARY_RELEASE})
set(ACE_LIBRARY_DEBUG ${ACE_ACE_LIBRARY_DEBUG})
set(ACE_LIBRARY ${ACE_ACE_LIBRARY})


########################################################################
# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(ACE PROPERTIES DESCRIPTION "The ADAPTIVE Communication Environment"
                                          URL "http://www.cs.wustl.edu/~schmidt/ACE.html")
endif()
