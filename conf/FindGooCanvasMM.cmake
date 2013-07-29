# Try to find the GooCanvasMM library.
# Once done this will define the following variables:
#
# GooCanvasMM_FOUND         - System has GooCanvasMM
# GooCanvasMM_INCLUDE_DIRS  - GooCanvasMM include directory
# GooCanvasMM_LIBRARIES     - GooCanvasMM libraries
# GooCanvasMM_DEFINITIONS   - Additional compiler flags for GooCanvasMM
# GooCanvasMM_VERSION       - GooCanvasMM version
# GooCanvasMM_MAJOR_VERSION - GooCanvasMM major version
# GooCanvasMM_MINOR_VERSION - GooCanvasMM minor version
# GooCanvasMM_PATCH_VERSION - GooCanvasMM patch version
# GooCanvasMM_TWEAK_VERSION - GooCanvasMM tweak version

# Copyright (C) 2012, 2013  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

include(MacroStandardFindModule)
macro_standard_find_module(GooCanvasMM goocanvasmm-1.0)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(GooCanvasMM PROPERTIES DESCRIPTION "A GtkMM wrapper for GooCanvas"
                                                  URL "http://live.gnome.org/GooCanvas")
endif()
