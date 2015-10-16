#.rst:
# MacroExtractVersion (Replaced by :module:`ExtractVersion`)
# ----------------------------------------------------------
#
# .. warning:: This module is deprecated. You should use :module:`ExtractVersion` instead.

#=============================================================================
# Copyright 2013-2014 iCub Facility, Istituto Italiano di Tecnologia
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


include(${YARP_MODULE_DIR}/YarpDeprecatedWarning.cmake)
yarp_deprecated_warning("MacroExtractVersion.cmake is deprecated. Use ExtractVersion instead.")

if(NOT YARP_NO_DEPRECATED)

    include(ExtractVersion)
    macro(MACRO_EXTRACT_VERSION)
        yarp_deprecated_warning("MACRO_EXTRACT_VERSION is deprecated. Use EXTRACT_VERSION instead")
        extract_version(${ARGN})
    endmacro()

endif()
