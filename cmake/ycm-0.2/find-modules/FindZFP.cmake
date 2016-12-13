# FindZFP
# ----------------
# Find the ZFP includes and library.
# Once done this will define the following variables::

# ZFP_INCLUDE_DIR     - ZFP include directory
# ZFP_LIBRARY         - ZFP libraries 
# ZFP_FOUND 					- if false, you cannot build anything that requires ZFP

#=============================================================================
# Copyright 2016 iCub Facility, Istituto Italiano di Tecnologia
#   Authors: Nicolo' Genesio <nicolo.genesio@iit.it>
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

include(FindPackageHandleStandardArgs)


if(WIN32)
  set(ZFP_DEBUG_POSTFIX "d")
  find_library(ZFP_DEBUG_LIBRARY NAMES ZFP${ZFP_DEBUG_POSTFIX} zfp${ZFP_DEBUG_POSTFIX} PATHS $ENV{ZFP_ROOT}/lib $ENV{ZFP_ROOT} DOC "ZFP library file (debug version)")
  find_path(ZFP_INCLUDE_DIR NAMES zfp.h HINTS $ENV{ZFP_ROOT}/inc)
  find_library(ZFP_LIBRARY NAMES ZFP libzfp HINTS $ENV{ZFP_ROOT}/lib)
else()
  find_path(ZFP_INCLUDE_DIR NAMES zfp.h HINTS $ENV{ZFP_ROOT}/inc)
  find_library(ZFP_LIBRARY NAMES ZFP libzfp.a HINTS $ENV{ZFP_ROOT}/lib)
endif()
 
 
if (ZFP_INCLUDE_DIR AND ZFP_LIBRARY) 
  set(ZFP_FOUND TRUE)
else () 
  set(ZFP_FOUND FALSE)
endif () 
 
if (ZFP_DEBUG_LIBRARY) 
  set(ZFP_DEBUG_FOUND TRUE)
else ()
  set(ZFP_DEBUG_LIBRARY ${ZFP_LIBRARY})
endif () 

find_package_handle_standard_args(ZFP FOUND_VAR ZFP_FOUND
                                  REQUIRED_VARS ZFP_LIBRARY ZFP_INCLUDE_DIR)

# TSS: backwards compatibility
set(ZFP_LIBRARIES ${ZFP_LIBRARY}) 
