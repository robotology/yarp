# Copyright (C) 2013  iCub Facility, Istituto Italiano di Tecnologia
# Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

macro(YARP_DEPRECATED_WARNING)
    if(NOT YARP_NO_DEPRECATED_WARNINGS)
        message(WARNING "${ARGN}")
    endif(NOT YARP_NO_DEPRECATED_WARNINGS)
endmacro()
