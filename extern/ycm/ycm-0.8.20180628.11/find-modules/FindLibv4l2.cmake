#.rst:
# FindLibv4l2
# -----------
#
# Try to find the libv4l2 library.
# Once done this will define the following variables::
#
#  Libv4l2_FOUND         - System has libv4l2
#  Libv4l2_INCLUDE_DIRS  - libv4l2 include directory
#  Libv4l2_LIBRARIES     - libv4l2 libraries
#  Libv4l2_DEFINITIONS   - Additional compiler flags for libv4l2
#  Libv4l2_VERSION       - libv4l2 version
#  Libv4l2_MAJOR_VERSION - libv4l2 major version
#  Libv4l2_MINOR_VERSION - libv4l2 minor version
#  Libv4l2_PATCH_VERSION - libv4l2 patch version
#  Libv4l2_TWEAK_VERSION - libv4l2 tweak version

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
standard_find_module(Libv4l2 libv4l2)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(Libv4l2 PROPERTIES DESCRIPTION "Video4Linux or V4L is a video capture and output device API and driver framework for the Linux kernel, supporting many USB webcams, TV tuners, and other devices."
                                              URL "https://www.kernel.org/doc/Documentation/video4linux/v4l2-framework.txt")
endif()
