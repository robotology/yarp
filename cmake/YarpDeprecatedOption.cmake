# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

function(YARP_DEPRECATED_OPTION _old)
  get_property(_old_set CACHE ${_old} PROPERTY VALUE SET)
  if(_old_set)
    message(DEPRECATION "\"${_old}\" is deprecated. Removing old cache entry.")
    unset(${_old} CACHE)
  endif()
endfunction()
