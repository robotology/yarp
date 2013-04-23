# Try to find the TinyXML library.
# Once done this will define the following variables:
#
# TinyXML_FOUND         - System has TinyXML
# TinyXML_INCLUDE_DIRS  - TinyXML include directory
# TinyXML_LIBRARIES     - TinyXML libraries
# TinyXML_DEFINITIONS   - Additional compiler flags for TinyXML
# TinyXML_VERSION       - TinyXML version
# TinyXML_MAJOR_VERSION - TinyXML major version
# TinyXML_MINOR_VERSION - TinyXML minor version
# TinyXML_PATCH_VERSION - TinyXML patch version

# Copyright (C) 2012, 2013  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

include(MacroStandardFindModule)
macro_standard_find_module(TinyXML tinyxml)
