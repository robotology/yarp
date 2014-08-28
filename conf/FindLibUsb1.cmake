# Try to find the libusb library.
# Once done this will define the following variables:
#
# LibUsb1_FOUND         - System has LibUsb1
# LibUsb1_INCLUDE_DIRS  - LibUsb1 include directory
# LibUsb1_LIBRARIES     - LibUsb1 libraries
# LibUsb1_DEFINITIONS   - Additional compiler flags for LibUsb1
# LibUsb1_VERSION       - LibUsb1 version
# LibUsb1_MAJOR_VERSION - LibUsb1 major version
# LibUsb1_MINOR_VERSION - LibUsb1 minor version
# LibUsb1_PATCH_VERSION - LibUsb1 patch version

# Copyright (C) 2012-2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

include(MacroStandardFindModule)
macro_standard_find_module(LibUsb1 libusb-1.0 SKIP_CMAKE_CONFIG)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(LibUsb1 PROPERTIES DESCRIPTION "Userspace USB programming library"
                                              URL "http://libusb.org/")
endif()
