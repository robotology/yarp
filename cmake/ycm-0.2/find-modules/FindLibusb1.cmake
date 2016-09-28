#.rst:
# FindLibusb1
# -----------
#
# Try to find the libusb-1 library.
# Once done this will define the following variables::
#
#  Libusb1_FOUND         - System has libusb-1
#  Libusb1_INCLUDE_DIRS  - libusb-1 include directory
#  Libusb1_LIBRARIES     - libusb-1 libraries
#  Libusb1_DEFINITIONS   - Additional compiler flags for libusb-1
#  Libusb1_VERSION       - libusb-1 version
#  Libusb1_MAJOR_VERSION - libusb-1 major version
#  Libusb1_MINOR_VERSION - libusb-1 minor version
#  Libusb1_PATCH_VERSION - libusb-1 patch version

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
standard_find_module(Libusb1 libusb-1.0 SKIP_CMAKE_CONFIG)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(Libusb1 PROPERTIES DESCRIPTION "Userspace USB programming library"
                                              URL "http://libusb.org/")
endif()
