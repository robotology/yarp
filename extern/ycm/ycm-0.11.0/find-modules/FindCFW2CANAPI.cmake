#.rst:
# FindCFW2CANAPI
# --------------
#
# Created::
#
#  CFW2CANAPI_INC_DIRS   - Directories to include to use esdcan api
#  CFW2CANAPI_LIB        - Default library to link against to use the esdcan API
#  CFF2CANAPI_FOUND      - If false, don't try to use esdcan API

#=============================================================================
# Copyright 2010 RobotCub Consortium
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

if(NOT CFW2CANAPI_FOUND)
    set(CFW2CANAPI_DIR $ENV{CFW2CANAPI_DIR} CACHE PATH "Path to CFW2CANAPI")
    set(CFW2CANAPI_INC_DIRS ${CFW2CANAPI_DIR}/LinuxDriver/API)

    if(WIN32)
       #sorry not available in windows
    else()
        find_library(CFW2CANAPI_LIB cfw002 ${CFW2CANAPI_DIR}/LinuxDriver/API)
    endif()

    if(CFW2CANAPI_LIB)
       set(CFW2CANAPI_FOUND TRUE)
    else()
       set(CFW2CANAPI_FOUND FALSE)
       set(CFW2CANAPI_INC_DIRS)
       set(CFW2CANAPI_LIB )
    endif(CFW2CANAPI_LIB)

endif(NOT CFW2CANAPI_FOUND)
