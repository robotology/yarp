# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

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
