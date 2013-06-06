# Try to find the GtkDatabox library.
# Once done this will define the following variables:
#
# GtkDatabox_FOUND         - System has GtkDatabox
# GtkDatabox_INCLUDE_DIRS  - GtkDatabox include directory
# GtkDatabox_LIBRARIES     - GtkDatabox libraries
# GtkDatabox_DEFINITIONS   - Additional compiler flags for GtkDatabox
# GtkDatabox_VERSION       - GtkDatabox version
# GtkDatabox_MAJOR_VERSION - GtkDatabox major version
# GtkDatabox_MINOR_VERSION - GtkDatabox minor version
# GtkDatabox_PATCH_VERSION - GtkDatabox patch version
# GtkDatabox_TWEAK_VERSION - GtkDatabox tweak version

# Copyright (C) 2012, 2013  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

include(MacroStandardFindModule)
macro_standard_find_module(GtkDatabox gtkdatabox)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(GtkDatabox PROPERTIES DESCRIPTION "A GTK+ widget for live display of large amounts of fluctuating numerical data"
                                                 URL "http://sourceforge.net/projects/gtkdatabox/")
endif()
