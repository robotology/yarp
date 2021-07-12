# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

function(YARP_RENAMED_OPTION _old _new)
  get_property(_old_set CACHE ${_old} PROPERTY VALUE SET)
  if(_old_set)
    message(DEPRECATION "\"${_old}\" is deprecated in favour of \"${_new}\". Updating new cache entry.")
    set_property(CACHE ${_new} PROPERTY VALUE ${${_old}})
    unset(${_old} CACHE)
  endif()
endfunction()
