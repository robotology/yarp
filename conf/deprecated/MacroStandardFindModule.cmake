#.rst:
# MacroStandardFindModule (Replaced by :module:`StandardFindModule`)
# ------------------------------------------------------------------
#
# .. warning:: This module is deprecated. You should use :module:`StandardFindModule` instead.

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
yarp_deprecated_warning("MacroStandardFindModule.cmake is deprecated. Use StandardFindModule instead.")

if(NOT YARP_NO_DEPRECATED)

    include(StandardFindModule)
    macro(MACRO_STANDARD_FIND_MODULE)
        yarp_deprecated_warning("MACRO_STANDARD_FIND_MODULE is deprecated. Use STANDARD_FIND_MODULE instead")
        standard_find_module(${ARGN})
    endmacro()

endif(NOT YARP_NO_DEPRECATED)
