#.rst:
# FindPLXCANAPI
# -------------
#
# Created::
#
#  PLXCANAPI_INC_DIRS   - Directories to include to use esdcan api
#  PLXCANAPI_LIB        - Default library to link against to use the esdcan API
#  PLXCANAPI_FOUND      - If false, don't try to use esdcan API

#=============================================================================
# Copyright 2009 RobotCub Consortium
#   Authors: Alexandre Bernardino <alex@isr.ist.utl.pt>
#            Paul Fitzpatrick <paulfitz@alum.mit.edu>
#            Lorenzo Natale <lorenzo.natale@iit.it>
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


IF(NOT PLXCANAPI_FOUND)
    SET(PLXCANAPI_DIR $ENV{PLXCANAPI_DIR} CACHE PATH "Path to PLXCANAPI")
    SET(PLXCANAPI_INC_DIRS ${PLXCANAPI_DIR}/include)

    IF(WIN32)
       #sorry not available in windows
    ELSE(WIN32)
       FIND_LIBRARY(PLXCANAPI_LIB PlxApi ${PLXCANAPI_DIR}/lib NO_DEFAULT_PATH)
    ENDIF(WIN32)

    IF(PLXCANAPI_LIB)
       SET(PLXCANAPI_FOUND TRUE)
    ELSE(PLXCANAPI_LIB)
       SET(PLXCANAPI_FOUND FALSE)
       SET(PLXCANAPI_INC_DIRS)
       SET(PLXCANAPI_LIB )
    ENDIF(PLXCANAPI_LIB)

ENDIF(NOT PLXCANAPI_FOUND)
