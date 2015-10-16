#.rst:
# FindFTDI
# --------
#
# Try to find ftdi.
# Once done this will define::
#
#  FTDI_FOUND - system has ftdi
#  FTDI_INCLUDE_DIR - ~ the ftdi include directory
#  FTDI_LIBRARY - Link these to use ftdi

#=============================================================================
# Copyright 2010 Ze Ji
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


include(FindPackageHandleStandardArgs)

find_path(FTDI_INCLUDE_DIR
          NAMES ftdi.h
          PATHS /usr/local/include
                /usr/include
                /usr/include/libftdi
                /usr/local/include/libftdi)

find_library(FTDI_LIBRARY
             NAMES ftdi
             PATHS /usr/lib
                   /usr/local/lib)

find_package_handle_standard_args(FTDI DEFAULT_MSG FTDI_LIBRARY)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(FTDI PROPERTIES DESCRIPTION "libFTDI - FTDI USB driver with bitbang mode")
    set_package_properties(FTDI PROPERTIES URL "http://www.intra2net.com/en/developer/libftdi/")
endif()
