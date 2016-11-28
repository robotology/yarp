# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

include(YarpDeprecatedWarning)

function(YARP_DEPRECATED_OPTION _old)
  get_property(_old_set CACHE ${_old} PROPERTY VALUE SET)
  if(_old_set)
    yarp_deprecated_warning("\"${_old}\" is deprecated. Removing old cache entry.")
    unset(${_old} CACHE)
  endif()
endfunction()
