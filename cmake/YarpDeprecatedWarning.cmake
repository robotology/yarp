# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(COMMAND YARP_DEPRECATED_WARNING)
  return()
endif()

macro(YARP_DEPRECATED_WARNING)
  if(NOT YARP_NO_DEPRECATED_WARNINGS)
    message(DEPRECATION "${ARGN}")
  endif()
endmacro()
