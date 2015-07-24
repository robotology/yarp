# Try to find the Fuse library.
# Once done this will define the following variables:
#
# Fuse_FOUND         - System has Fuse
# Fuse_INCLUDE_DIRS  - Fuse include directory
# Fuse_LIBRARIES     - Fuse libraries
# Fuse_DEFINITIONS   - Additional compiler flags for Fuse
# Fuse_VERSION       - Fuse version
# Fuse_MAJOR_VERSION - Fuse major version
# Fuse_MINOR_VERSION - Fuse minor version
# Fuse_PATCH_VERSION - Fuse patch version
# Fuse_TWEAK_VERSION - Fuse tweak version

# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

include(MacroStandardFindModule)
macro_standard_find_module(Fuse fuse)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(Fuse PROPERTIES DESCRIPTION "A simple interface for userspace programs to export a virtual filesystem to the Linux kernel"
                                           URL "http://fuse.sourceforge.net/")
endif()

