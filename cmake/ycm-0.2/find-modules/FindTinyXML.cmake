#.rst:
# FindTinyXML
# -----------
#
# Try to find the TinyXML library.
# Once done this will define the following variables::
#
#  TinyXML_FOUND         - System has TinyXML
#  TinyXML_INCLUDE_DIRS  - TinyXML include directory
#  TinyXML_LIBRARIES     - TinyXML libraries
#  TinyXML_DEFINITIONS   - Additional compiler flags for TinyXML
#  TinyXML_VERSION       - TinyXML version
#  TinyXML_MAJOR_VERSION - TinyXML major version
#  TinyXML_MINOR_VERSION - TinyXML minor version
#  TinyXML_PATCH_VERSION - TinyXML patch version

#=============================================================================
# Copyright 2012-2013  iCub Facility, Istituto Italiano di Tecnologia
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
standard_find_module(TinyXML tinyxml)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(TinyXML PROPERTIES DESCRIPTION "A small, simple XML parser for the C++ language"
                                              URL "http://www.grinninglizard.com/tinyxml/index.html")
endif()
