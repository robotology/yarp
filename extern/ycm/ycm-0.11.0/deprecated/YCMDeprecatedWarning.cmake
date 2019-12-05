# This module should not be used outside YCM.

#=============================================================================
# Copyright 2013-2014 Istituto Italiano di Tecnologia (IIT)
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


option(YCM_NO_DEPRECATED_WARNINGS "Do not print warnings when using deprecated modules." FALSE)
mark_as_advanced(YARP_NO_DEPRECATED_WARNING)

macro(YCM_DEPRECATED_WARNING)
    if(NOT YCM_NO_DEPRECATED_WARNINGS)
        message(WARNING "${ARGN}")
    endif(NOT YCM_NO_DEPRECATED_WARNINGS)
endmacro()
