# Try to find the ACE library
#
# Variables set:
#   ACE_FOUND
#   ACE_LIBRARIES
#   ACE_INCLUDE_DIRS

# Copyright: (C) 2009 RobotCub Consortium
# Authors: Alexandre Bernardino, Paul Fitzpatrick, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


## script does not work if executed twice bc ACE_LIBRARY get appended
# fix "optimized.lib" problem in windows (Lorenzo Natale)
if(NOT ACE_FOUND)

    ########################################################################
    ##  general find

    find_path(ACE_INCLUDE_DIR ace/ACE.h $ENV{ACE_ROOT} $ENV{ACE_ROOT}/include ${CMAKE_SOURCE_DIR}/../ACE_wrappers/ /usr/include /usr/local/include DOC "directory containing ace/*.h for ACE library")

    # in YARP1, config was in another directory
    set(ACE_INCLUDE_CONFIG_DIR "" CACHE STRING "location of ace/config.h")
    mark_as_advanced(ACE_INCLUDE_CONFIG_DIR)

    find_library(ACE_LIBRARY NAMES ACE ace PATHS $ENV{ACE_ROOT}/lib $ENV{ACE_ROOT} ${CMAKE_SOURCE_DIR}/../ACE_wrappers/lib/ /usr/lib /usr/local/lib DOC "ACE library file")

    if(WIN32 AND NOT CYGWIN)
        set(CMAKE_DEBUG_POSTFIX "d")
        find_library(ACE_DEBUG_LIBRARY NAMES ACE${CMAKE_DEBUG_POSTFIX} ace${CMAKE_DEBUG_POSTFIX} PATHS $ENV{ACE_ROOT}/lib $ENV{ACE_ROOT} ${CMAKE_SOURCE_DIR}/../ACE_wrappers/lib/ /usr/lib /usr/local/lib DOC "ACE library file (debug version)")
    endif(WIN32 AND NOT CYGWIN)

    if(ACE_DEBUG_LIBRARY)
        set(ACE_DEBUG_FOUND TRUE)
    endif(ACE_DEBUG_LIBRARY)

    if(ACE_DEBUG_FOUND)
        if(ACE_LIBRARY)
            set(ACE_LIBRARY optimized ${ACE_LIBRARY} debug ${ACE_DEBUG_LIBRARY})
        else (ACE_LIBRARY)
            set(ACE_LIBRARY debug ${ACE_DEBUG_LIBRARY})
        endif(ACE_LIBRARY)
    endif(ACE_DEBUG_FOUND)


    ########################################################################
    ## OS-specific extra linkage

    # Solaris needs some extra libraries that may not have been found already
    if(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
        message(STATUS "need to link solaris-specific libraries")
        #  link_libraries(socket rt)
        set(ACE_LIBRARY socket rt nsl ${ACE_LIBRARY})
    endif(CMAKE_SYSTEM_NAME STREQUAL "SunOS")

    # ACE package doesn't specify that pthread and rt are needed, which is
    # a problem for users of GoLD.  Link pthread (just on Linux for now).
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(ACE_LIBRARY ${ACE_LIBRARY} pthread rt)
    endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

    # Windows needs some extra libraries
    if(WIN32 AND NOT CYGWIN)
        message(STATUS "need to link windows-specific libraries")
        # if ACE found, add winmm dependency
        if(ACE_LIBRARY)
            set(ACE_LIBRARY winmm ${ACE_LIBRARY})
        endif(ACE_LIBRARY)
    endif(WIN32 AND NOT CYGWIN)

    if(MINGW)
        message(STATUS "need to link windows-specific libraries")
        #link_libraries(winmm wsock32)
        set(ACE_LIBRARY winmm ws2_32 wsock32 ${ACE_LIBRARY})
    endif(MINGW)


    ########################################################################
    ## finished - now just set up flags and complain to user if necessary

    if(ACE_INCLUDE_DIR AND ACE_LIBRARY)
        set(ACE_FOUND TRUE)
    else(ACE_INCLUDE_DIR AND ACE_LIBRARY)
        set(ACE_FOUND FALSE)
    endif(ACE_INCLUDE_DIR AND ACE_LIBRARY)

    if(ACE_FOUND)
        if(NOT ACE_FIND_QUIETLY)
            message(STATUS "Found ACE library: ${ACE_LIBRARY} (${CMAKE_SYSTEM_NAME})")
            message(STATUS "Found ACE include: ${ACE_INCLUDE_DIR}")
        endif(NOT ACE_FIND_QUIETLY)
    else(ACE_FOUND)
        if(ACE_FIND_REQUIRED)
            message(FATAL_ERROR "Could not find ACE")
        endif(ACE_FIND_REQUIRED)
    endif(ACE_FOUND)

    set(ACE_LIBRARIES ${ACE_LIBRARY})

    if(NOT ACE_LIBRARIES)
        set(ACE_LIBRARIES ${ACE_LIBRARY})
    endif(NOT ACE_LIBRARIES)
    if(NOT ACE_INCLUDE_DIRS)
        set(ACE_INCLUDE_DIRS ${ACE_INCLUDE_DIR} ${ACE_INCLUDE_CONFIG_DIR})
    endif(NOT ACE_INCLUDE_DIRS)

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(ACE DEFAULT_MSG ACE_LIBRARIES ACE_INCLUDE_DIRS)
endif()
