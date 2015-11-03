#.rst:
# FindGtkDataboxMM
# ----------------
#
# Try to find the GtkDataboxMM library.
# Once done this will define the following variables::
#
#  GtkDataboxMM_FOUND         - System has GtkDataboxMM
#  GtkDataboxMM_INCLUDE_DIRS  - GtkDataboxMM include directory
#  GtkDataboxMM_LIBRARIES     - GtkDataboxMM libraries
#  GtkDataboxMM_DEFINITIONS   - Additional compiler flags for GtkDataboxMM
#  GtkDataboxMM_VERSION       - GtkDataboxMM version
#  GtkDataboxMM_MAJOR_VERSION - GtkDataboxMM major version
#  GtkDataboxMM_MINOR_VERSION - GtkDataboxMM minor version
#  GtkDataboxMM_PATCH_VERSION - GtkDataboxMM patch version
#  GtkDataboxMM_TWEAK_VERSION - GtkDataboxMM tweak version
#
# If the library is found, the imported target ``GtkDataboxMM::gtkdataboxmm`` is
# created.

#=============================================================================
# Copyright 2012-2015 iCub Facility, Istituto Italiano di Tecnologia
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
include(ReplaceImportedTargets)
include(CMakeFindDependencyMacro)

find_dependency(GTK2)
find_dependency(GtkDatabox)

standard_find_module(GtkDataboxMM gtkdataboxmm-0.9
                     TARGET GtkDataboxMM::gtkdataboxmm
                     REPLACE_TARGETS ${GTK2_LIBRARIES}
                                     ${GtkDatabox_LIBRARIES})

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(GtkDataboxMM PROPERTIES DESCRIPTION "A GtkMM wrapper for GtkDatabox"
                                                   URL "http://sourceforge.net/projects/gtkdataboxmm/")
endif()
