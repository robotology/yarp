# Try to find the OpenNI library.
# Once done this will define the following variables:
#
# OpenNI_FOUND         - System has OpenNI
# OpenNI_INCLUDE_DIRS  - OpenNI include directory
# OpenNI_LIBRARIES     - OpenNI libraries
# OpenNI_DEFINITIONS   - Additional compiler flags for OpenNI
# OpenNI_VERSION       - OpenNI version
# OpenNI_MAJOR_VERSION - OpenNI major version
# OpenNI_MINOR_VERSION - OpenNI minor version
# OpenNI_PATCH_VERSION - OpenNI patch version

# Copyright (C) 2012-2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

include(MacroStandardFindModule)
macro_standard_find_module(OpenNI libopenni SKIP_CMAKE_CONFIG)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(OpenNI PROPERTIES DESCRIPTION "Open Natural Interaction framework"
                                             URL "https://github.com/OpenNI/OpenNI")
endif()
