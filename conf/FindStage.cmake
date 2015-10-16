#.rst:
# FindStage
# ------------
#
# Try to find Stage, a library for easy editing of command lines.
# Once done this will define the following variables::
#
#  Stage_FOUND         - System has Stage
#  Stage_INCLUDE_DIRS  - Stage include directory
#  Stage_LIBRARIES     - Stage libraries
#  Stage_DEFINITIONS   - Additional compiler flags for Stage
#  Stage_VERSION       - Stage version
#  Stage_MAJOR_VERSION - Stage major version
#  Stage_MINOR_VERSION - Stage minor version
#
# Environment variables used to locate the Stage library:
# READLINE_DIR - Stage root directory
#
# Cached variables used to locate the Stage library:
# Stage_INCLUDE_DIR - the Stage include directory
# Stage_LIBRARY_RELEASE - Stage library (release)
# Stage_LIBRARY_DEBUG - Stage library (debug)

#=============================================================================
# Copyright 2009 RobotCub Consortium
# Copyright 2014 iCub Facility, Istituto Italiano di Tecnologia
#   Authors: Paul Fitzpatrick <paulfitz@alum.mit.edu>
#            Daniele E. Domenichelli <daniele.domenichelli@iit.it>
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
standard_find_module(Stage stage SKIP_CMAKE_CONFIG)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(Stage PROPERTIES DESCRIPTION "Mobile robot simulator"
                                            URL "https://rtv.github.io/Stage/")
endif()
