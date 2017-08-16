# Copyright (C) 2013, 2017 Istituto Italiano di Tecnologia (IIT)
# Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMMAND YARP_DEPRECATED_WARNING)
  return()
endif()

macro(YARP_DEPRECATED_WARNING)
  if(NOT YARP_NO_DEPRECATED_WARNINGS)
    message(DEPRECATION "${ARGN}")
  endif()
endmacro()
