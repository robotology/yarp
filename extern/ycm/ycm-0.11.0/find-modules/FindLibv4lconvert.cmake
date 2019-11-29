#.rst:
# FindLibv4lconvert
# -----------------
#
# Try to find the libv4lconvert library.
# Once done this will define the following variables::
#
#  Libv4lconvert_FOUND         - System has libv4lconvert
#  Libv4lconvert_INCLUDE_DIRS  - libv4lconvert include directory
#  Libv4lconvert_LIBRARIES     - libv4lconvert libraries
#  Libv4lconvert_DEFINITIONS   - Additional compiler flags for libv4lconvert
#  Libv4lconvert_VERSION       - libv4lconvert version
#  Libv4lconvert_MAJOR_VERSION - libv4lconvert major version
#  Libv4lconvert_MINOR_VERSION - libv4lconvert minor version
#  Libv4lconvert_PATCH_VERSION - libv4lconvert patch version
#  Libv4lconvert_TWEAK_VERSION - libv4lconvert tweak version

#=============================================================================
# Copyright 2015 Istituto Italiano di Tecnologia (IIT)
#   Authors: Alberto Cardellino <alberto.cardellino@iit.it>
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
standard_find_module(Libv4lconvert libv4lconvert)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(Libv4lconvert PROPERTIES DESCRIPTION "Video4linux frame format conversion library")
endif()
