# Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
# Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

macro(YARP_BACKUP_VARIABLE _var)
  unset(_${_var})
  if(DEFINED ${_var})
    set(_${_var} "${${_var}}")
  endif()
endmacro()

macro(YARP_RESTORE_VARIABLE _var)
  unset(${_var})
  if(DEFINED _${_var})
    set(${_var} "${_${_var}}")
  endif()
  unset(_${_var})
endmacro()
