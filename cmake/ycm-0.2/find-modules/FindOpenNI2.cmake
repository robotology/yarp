#.rst:
# FindOpenNI
# ----------
#
# Try to find the OpenNI library.
# Once done this will define the following variables::
#
#  OpenNI2_FOUND         - System has OpenNI
#  OpenNI2_INCLUDE_DIRS  - OpenNI include directory
#  OpenNI2_LIBRARIES     - OpenNI libraries
#  OpenNI2_DEFINITIONS   - Additional compiler flags for OpenNI
#  OpenNI2_VERSION       - OpenNI2 version
#  OpenNI2_MAJOR_VERSION - OpenNI2 major version
#  OpenNI2_MINOR_VERSION - OpenNI2 minor version
#  OpenNI2_PATCH_VERSION - OpenNI2 patch version

#=============================================================================
# Copyright 2012-2014 iCub Facility, Istituto Italiano di Tecnologia
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
standard_find_module(OpenNI2 libopenni SKIP_CMAKE_CONFIG)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(OpenNI2 PROPERTIES DESCRIPTION "Open Natural Interaction framework"
                                             URL "https://github.com/OpenNI/OpenNI2")
endif()
