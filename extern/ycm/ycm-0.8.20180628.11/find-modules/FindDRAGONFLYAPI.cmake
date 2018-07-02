#.rst:
# FindDRAGONFLYAPI
# ----------------
#
# Created::
#
#  DRAGONFLYAPI_INC_DIRS   - Directories to include to use dragonfly API
#  DRAGONFLYAPI_LIB        - Default library to link against to use the dragonfly API
#  DRAGONFLYAPI_FOUND      - If false, don't try to use dragonfly API

#=============================================================================
# Copyright 2009 RobotCub Consortium
#   Authors: Lorenzo Natale <lorenzo.natale@iit.it>
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

if(NOT DRAGONFLYAPI_FOUND)
    if(WIN32)
        set(DRAGONFLYAPI_DIR $ENV{DRAGONFLYAPI_DIR} CACHE PATH "Path to DRAGONFLYAPI")
        mark_as_advanced(DRAGONFLYAPI_DIR)

        find_library(DRAGONFLYAPI_LIB FlyCapture2 ${DRAGONFLYAPI_DIR}/lib
                                                  "C:\\Program Files\\Point Grey Research\\FlyCapture2\\lib")
        find_path(DRAGONFLYAPI_INC_DIRS FlyCapture2.h ${DRAGONFLYAPI_DIR}/include
                                                      "C:\\Program Files\\Point Grey Research\\FlyCapture2\\include")
    else()
        find_library(lib_raw1394 raw1394)
        find_library(lib_dc1394 dc1394)
        if(lib_raw1394 AND lib_dc1394)
            set(DRAGONFLYAPI_LIB ${lib_raw1394} ${lib_dc1394})
        endif()
    endif()

    message(STATUS ${DRAGONFLYAPI_LIB} ${DRAGONFLYAPI_INC_DIRS})

    if(DRAGONFLYAPI_LIB)
        if(WIN32)
            if(DRAGONFLYAPI_INC_DIRS)
                set(DRAGONFLYAPI_FOUND TRUE)
            else()
                set(DRAGONFLYAPI_FOUND FALSE)
            endif()
        else()
            set(DRAGONFLYAPI_FOUND TRUE)
        endif()
    else(DRAGONFLYAPI_LIB)
        set(DRAGONFLYAPI_FOUND FALSE)
    endif()
endif()
