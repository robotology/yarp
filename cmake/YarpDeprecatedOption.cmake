# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

function(YARP_DEPRECATED_OPTION _old)
  get_property(_old_set CACHE ${_old} PROPERTY VALUE SET)
  if(_old_set)
    message(DEPRECATION "\"${_old}\" is deprecated. Removing old cache entry.")
    unset(${_old} CACHE)
  endif()
endfunction()
