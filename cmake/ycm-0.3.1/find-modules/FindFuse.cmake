#.rst:
# FindFuse
# ----------------
#
# Try to find the Fuse library.
# Once done this will define the following variables::
#
#  Fuse_FOUND         - System has Fuse
#  Fuse_INCLUDE_DIRS  - Fuse include directory
#  Fuse_LIBRARIES     - Fuse libraries
#  Fuse_DEFINITIONS   - Additional compiler flags for Fuse
#  Fuse_VERSION       - Fuse version
#  Fuse_MAJOR_VERSION - Fuse major version
#  Fuse_MINOR_VERSION - Fuse minor version
#  Fuse_PATCH_VERSION - Fuse patch version
#  Fuse_TWEAK_VERSION - Fuse tweak version

#=============================================================================
# Copyright 2014 iCub Facility, Istituto Italiano di Tecnologia
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
standard_find_module(Fuse fuse)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(Fuse PROPERTIES DESCRIPTION "A simple interface for userspace programs to export a virtual filesystem to the Linux kernel"
                                           URL "http://fuse.sourceforge.net/")
endif()
