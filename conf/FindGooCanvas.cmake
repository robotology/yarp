#.rst:
# FindGooCanvas
# -------------
#
# Try to find the GooCanvas library.
# Once done this will define the following variables::
#
#  GooCanvas_FOUND         - System has GooCanvas
#  GooCanvas_INCLUDE_DIRS  - GooCanvas include directory
#  GooCanvas_LIBRARIES     - GooCanvas libraries
#  GooCanvas_DEFINITIONS   - Additional compiler flags for GooCanvas
#  GooCanvas_VERSION       - GooCanvas version
#  GooCanvas_MAJOR_VERSION - GooCanvas major version
#  GooCanvas_MINOR_VERSION - GooCanvas minor version
#  GooCanvas_PATCH_VERSION - GooCanvas patch version
#  GooCanvas_TWEAK_VERSION - GooCanvas tweak version

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

standard_find_module(GooCanvas goocanvas
                     TARGET GooCanvas::goocanvas
                     REPLACE_TARGETS ${GTK2_LIBRARIES})

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(GooCanvas PROPERTIES DESCRIPTION "A canvas widget for GTK+ that uses the cairo 2D library for drawing"
                                                URL "https://live.gnome.org/GooCanvas")
endif()
