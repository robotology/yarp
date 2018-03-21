# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

include(YarpDeprecatedWarning)

function(YARP_RENAMED_OPTION _old _new)
  get_property(_old_set CACHE ${_old} PROPERTY VALUE SET)
  if(_old_set)
    yarp_deprecated_warning("\"${_old}\" is deprecated in favour of \"${_new}\". Updating new cache entry.")
    set_property(CACHE ${_new} PROPERTY VALUE ${${_old}})
    unset(${_old} CACHE)
  endif()
endfunction()
