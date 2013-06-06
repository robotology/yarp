# Try to find the GooCanvas library.
# Once done this will define the following variables:
#
# GooCanvas_FOUND         - System has GooCanvas
# GooCanvas_INCLUDE_DIRS  - GooCanvas include directory
# GooCanvas_LIBRARIES     - GooCanvas libraries
# GooCanvas_DEFINITIONS   - Additional compiler flags for GooCanvas
# GooCanvas_VERSION       - GooCanvas version
# GooCanvas_MAJOR_VERSION - GooCanvas major version
# GooCanvas_MINOR_VERSION - GooCanvas minor version
# GooCanvas_PATCH_VERSION - GooCanvas patch version
# GooCanvas_TWEAK_VERSION - GooCanvas tweak version

# Copyright (C) 2012, 2013  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

include(MacroStandardFindModule)
macro_standard_find_module(GooCanvas goocanvas)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(GooCanvas PROPERTIES DESCRIPTION "A canvas widget for GTK+ that uses the cairo 2D library for drawing"
                                                URL "https://live.gnome.org/GooCanvas")
endif()
