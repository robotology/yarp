#.rst:
# FindGooCanvasMM
# ---------------
#
# Try to find the GooCanvasMM library.
# Once done this will define the following variables::
#
#  GooCanvasMM_FOUND         - System has GooCanvasMM
#  GooCanvasMM_INCLUDE_DIRS  - GooCanvasMM include directory
#  GooCanvasMM_LIBRARIES     - GooCanvasMM libraries
#  GooCanvasMM_DEFINITIONS   - Additional compiler flags for GooCanvasMM
#  GooCanvasMM_VERSION       - GooCanvasMM version
#  GooCanvasMM_MAJOR_VERSION - GooCanvasMM major version
#  GooCanvasMM_MINOR_VERSION - GooCanvasMM minor version
#  GooCanvasMM_PATCH_VERSION - GooCanvasMM patch version
#  GooCanvasMM_TWEAK_VERSION - GooCanvasMM tweak version

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
find_dependency(GooCanvas)

standard_find_module(GooCanvasMM goocanvasmm-1.0
                     TARGET GooCanvasMM::goocanvasmm
                     REPLACE_TARGETS ${GTK2_LIBRARIES}
                                     ${GooCanvas_LIBRARIES})

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(GooCanvasMM PROPERTIES DESCRIPTION "A GtkMM wrapper for GooCanvas"
                                                  URL "http://live.gnome.org/GooCanvas")
endif()
