# Try to find the GtkDataboxMM library.
# Once done this will define the following variables:
#
# GtkDataboxMM_FOUND         - System has GtkDataboxMM
# GtkDataboxMM_INCLUDE_DIRS  - GtkDataboxMM include directory
# GtkDataboxMM_LIBRARIES     - GtkDataboxMM libraries
# GtkDataboxMM_DEFINITIONS   - Additional compiler flags for GtkDataboxMM
# GtkDataboxMM_VERSION       - GtkDataboxMM version
# GtkDataboxMM_MAJOR_VERSION - GtkDataboxMM major version
# GtkDataboxMM_MINOR_VERSION - GtkDataboxMM minor version
# GtkDataboxMM_PATCH_VERSION - GtkDataboxMM patch version
# GtkDataboxMM_TWEAK_VERSION - GtkDataboxMM tweak version

# Copyright (C) 2012, 2013  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

include(MacroStandardFindModule)
macro_standard_find_module(GtkDataboxMM gtkdataboxmm-0.9)
