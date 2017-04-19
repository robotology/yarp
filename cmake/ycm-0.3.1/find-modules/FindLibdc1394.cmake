#.rst:
# FindLibdc1394
# -------------
#
# Try to find the libdc1394 library.
# Once done this will define the following variables::
#
#  Libdc1394_FOUND         - System has libdc1394
#  Libdc1394_INCLUDE_DIRS  - libdc1394 include directory
#  Libdc1394_LIBRARIES     - libdc1394 libraries
#  Libdc1394_DEFINITIONS   - Additional compiler flags for libdc1394
#  Libdc1394_VERSION       - libdc1394 version
#  Libdc1394_MAJOR_VERSION - libdc1394 major version
#  Libdc1394_MINOR_VERSION - libdc1394 minor version
#  Libdc1394_PATCH_VERSION - libdc1394 patch version

#=============================================================================
# Copyright 2012-2015  iCub Facility, Istituto Italiano di Tecnologia
#   Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
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
standard_find_module(Libdc1394 libdc1394-2)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(Libdc1394 PROPERTIES DESCRIPTION "High level programming interface for IEEE1394 digital camera"
                                                URL "http://damien.douxchamps.net/ieee1394/libdc1394/")
endif()
