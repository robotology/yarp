# Try to find the ACE library
#
# Variables set:
#   ACE_FOUND
#   ACE_LIBRARIES
#   ACE_INCLUDE_DIRS

# Copyright: (C) 2009 RobotCub Consortium
# Copyright: (C) 2013  iCub Facility, Istituto Italiano di Tecnologia
# Authors: Alexandre Bernardino, Paul Fitzpatrick, Lorenzo Natale, Daniele E. Domenichelli
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


include(MacroStandardFindModule)
macro_standard_find_module(ACE ACE SKIP_CMAKE_CONFIG NOT_REQUIRED)


## script does not work if executed twice bc ACE_LIBRARY get appended
# fix "optimized.lib" problem in windows (Lorenzo Natale)
if(NOT ACE_FOUND)

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

    find_library(ACE_LIBRARY_RELEASE
                 NAMES ACE
                       ace
                 PATHS $ENV{ACE_ROOT}/lib
                       $ENV{ACE_ROOT}
                       /usr/lib
                       /usr/local/lib
                 DOC "ACE library file")

    set(CMAKE_DEBUG_POSTFIX "d")
    find_library(ACE_LIBRARY_DEBUG
                 NAMES ACE${CMAKE_DEBUG_POSTFIX}
                       ace${CMAKE_DEBUG_POSTFIX}
                 PATHS $ENV{ACE_ROOT}/lib
                       $ENV{ACE_ROOT}
                       /usr/lib
                       /usr/local/lib
                 DOC "ACE library file (debug version)")
    include(SelectLibraryConfigurations)
    select_library_configurations(ACE)

    set(ACE_LIBRARIES ${ACE_LIBRARY})
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
    find_package_handle_standard_args(ACE DEFAULT_MSG ACE_LIBRARIES ACE_INCLUDE_DIRS)
endif()

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(ACE PROPERTIES DESCRIPTION "The ADAPTIVE Communication Environment"
                                          URL "http://www.cs.wustl.edu/~schmidt/ACE.html")
endif()
