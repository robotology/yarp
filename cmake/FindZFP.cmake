#.rst:
# FindZFP
# -------
#
# Find the ZFP floating point compression library.
#
# Once done this will define the following variables::
#
#   ZFP_INCLUDE_DIRS    - ZFP include directory
#   ZFP_LIBRARIES       - ZFP libraries 
#   ZFP_FOUND           - if false, you cannot build anything that requires ZFP

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
include(SelectLibraryConfigurations)

find_path(ZFP_INCLUDE_DIR
          NAMES zfp.h
          PATHS $ENV{ZFP_ROOT}/inc
          DOC "ZFP include directory")
find_library(ZFP_LIBRARY_RELEASE
             NAMES ZFP zfp
             PATHS $ENV{ZFP_ROOT}/lib
             DOC "ZFP library file (release version)")
find_library(ZFP_LIBRARY_DEBUG
             NAMES ZFPd zfpd
             PATHS $ENV{ZFP_ROOT}/lib
             DOC "ZFP library file (debug version)") 

mark_as_advanced(ZFP_INCLUDE_DIR
                 ZFP_LIBRARY_RELEASE
                 ZFP_LIBRARY_DEBUG)

select_library_configurations(ZFP)

set(ZFP_LIBRARIES ${ZFP_LIBRARY})
set(ZFP_INCLUDE_DIRS ${ZFP_INCLUDE_DIR}) 

find_package_handle_standard_args(ZFP
                                  FOUND_VAR ZFP_FOUND
                                  REQUIRED_VARS ZFP_LIBRARIES ZFP_INCLUDE_DIRS)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(ZFP PROPERTIES DESCRIPTION "An open source C/C++ library for compressed floating-point arrays"
                                          URL "http://computation.llnl.gov/projects/floating-point-compression")
endif()

