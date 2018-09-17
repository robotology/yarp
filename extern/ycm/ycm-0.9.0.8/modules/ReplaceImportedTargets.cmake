#.rst:
# ReplaceImportedTargets
# ----------------------
#
# Adds the :command:`replace_imported_targets` command that useful to
# replace paths with imported targets in link variables (like
# ``<FOO>_LIBRARIES``) and targets.
#
# .. command:: replace_imported_targets
#
# Replace imported targets in a list of and targets and paths::
#
#  replace_imported_targets(<var> [target [target [...]]])
#
# Each path in ``var`` corrisponding to one of the ``targets`` will be
# replaced with the corrisponding ``target``, taking care to remove the
# relative ``optimized`` and ``debug`` keywords.
#
# For each existing target in ``var``, the following properties will be
# searched for imported locations of targets, and, if set, will be
# replaced in the same way::
#
#  IMPORTED_LINK_DEPENDENT_LIBRARIES
#  IMPORTED_LINK_DEPENDENT_LIBRARIES_<CONFIG>
#  IMPORTED_LINK_INTERFACE_LIBRARIES
#  IMPORTED_LINK_INTERFACE_LIBRARIES_<CONFIG>
#  INTERFACE_LINK_LIBRARIES
#  LINK_INTERFACE_LIBRARIES
#  LINK_INTERFACE_LIBRARIES_<CONFIG>
#  LINK_LIBRARIES

#=============================================================================
# Copyright 2015 Istituto Italiano di Tecnologia (IIT)
#   Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of YCM, substitute the full
#  License text for the above reference.)


function(_REPLACE_IMPORTED_TARGETS_INTERNAL _var)
  foreach(_target ${ARGN})
    if(TARGET ${_target})
      get_target_property(_imported ${_target} IMPORTED)
      if(_imported)
        get_target_property(_configurations ${_target} IMPORTED_CONFIGURATIONS)
        foreach(_conf ${_configurations})
          get_target_property(_location ${_target} IMPORTED_LOCATION_${_conf})
          string(REGEX REPLACE "((optimized|debug);)?${_location}" ${_target} ${_var} "${${_var}}")
        endforeach()
      endif()
    endif()
  endforeach()
  list(REMOVE_DUPLICATES ${_var})
  set(${_var} ${${_var}} PARENT_SCOPE)
endfunction()


function(REPLACE_IMPORTED_TARGETS _var)
  set(_replace_targets ${ARGN})

  if(TARGET ${_var})
    set(_tgt ${_var})
  else()
    set(_tgt ${${_var}})
  endif()
  foreach(_target ${_tgt})
    if(TARGET ${_target})

      # Replace in all link related properties.
      set(_properties IMPORTED_LINK_DEPENDENT_LIBRARIES
                      IMPORTED_LINK_INTERFACE_LIBRARIES
                      INTERFACE_LINK_LIBRARIES
                      LINK_INTERFACE_LIBRARIES
                      LINK_LIBRARIES)
      get_property(_configurations TARGET ${_target} PROPERTY IMPORTED_CONFIGURATIONS)
      foreach(_config ${_configurations})
        list(APPEND _properties IMPORTED_LINK_DEPENDENT_LIBRARIES_${_config}
                                IMPORTED_LINK_INTERFACE_LIBRARIES_${_config}
                                LINK_INTERFACE_LIBRARIES_${_config})
      endforeach()

      foreach(_prop ${_properties})
        get_property(_prop_set TARGET ${_target} PROPERTY ${_prop} SET)
        if(_prop_set)
          get_property(_prop_value TARGET ${_target} PROPERTY ${_prop})
          _replace_imported_targets_internal(_prop_value ${_replace_targets})

          # CMake 3.0.x throws an exception when setting an empty
          # property. All the other versions are fine with it.
          # Remove this check when CMake minimum required version is
          # 3.1 or higher.
          if(NOT ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" VERSION_EQUAL 3.0 AND "${_prop_value}" STREQUAL ""))
            set_property(TARGET ${_target} PROPERTY ${_prop} ${_prop_value})
          endif()
        endif()
      endforeach()
    endif()
  endforeach()

  if(NOT TARGET ${_var})
    # replace in the variable.
    _replace_imported_targets_internal(${_var} ${_replace_targets})
    set(${_var} ${${_var}} PARENT_SCOPE)
  endif()

endfunction()
