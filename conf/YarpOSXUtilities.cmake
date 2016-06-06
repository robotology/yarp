# Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
# Authors: Francesco Romano <francesco.romano@iit.it>
#          Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


include(GetAllCMakeProperties)

function(YARP_OSX_DUPLICATE_AND_ADD_BUNDLE)

 if(APPLE)
    set(_options )
    set(_oneValueArgs TARGET_ORIG
                      TARGET_DEST
                      APP_ICON
                      INSTALL_DESTINATION
                      INSTALL_COMPONENT)
    set(_multiValueArgs )

    cmake_parse_arguments(_DADB "${_options}"
                                "${_oneValueArgs}"
                                "${_multiValueArgs}"
                                "${ARGN}")

    if(NOT DEFINED _DADB_TARGET_ORIG)
      message(FATAL_ERROR "TARGET_ORIG is required")
    endif()
    set(_target_orig ${_DADB_TARGET_ORIG})

    if(NOT DEFINED _DADB_TARGET_DEST)
      set(_target_dest ${_target_orig}.app)
    else()
      set(_target_dest ${_DADB_TARGET_DEST})
    endif()

    if(DEFINED _DADB_INSTALL_COMPONENT AND NOT DEFINED _DADB_INSTALL_DESTINATION)
      message(FATAL_ERROR "INSTALL_COMPONENT cannot be used without INSTALL_DESTINATION")
    endif()

    if(DEFINED _DADB_UNPARSED_ARGUMENTS)
      message(FATAL_ERROR "Unknown arguments ${_DADB_UNPARSED_ARGUMENTS}")
    endif()

    get_all_cmake_properties(_all_properties)

    # Cleanup property list of all that should not be checked
    foreach(_prop_name ${_all_properties})
      if("${_prop_name}" STREQUAL "TYPE")
        # Ensure that the target is an executable
        list(REMOVE_ITEM _all_properties "${_prop_name}")
        get_target_property(_prop_value ${_target_orig} ${_prop_name})
        if(NOT "${_prop_value}" STREQUAL "EXECUTABLE")
          message(FATAL_ERROR "Not an executable")
        endif()
      elseif("${_prop_name}" STREQUAL "IMPORTED")
        # Ensure that this is not an imported target
        list(REMOVE_ITEM _all_properties "${_prop_name}")
        get_target_property(_prop_value ${_target_orig} ${_prop_name})
        if(_prop_value)
          message(FATAL_ERROR "This is an imported target")
        endif()
      elseif("${_prop_name}" STREQUAL "NAME")
        # Ensure that the target NAME property is the same as the one we expect
        list(REMOVE_ITEM _all_properties "${_prop_name}")
        get_target_property(_prop_value ${_target_orig} ${_prop_name})
        if(NOT "${_prop_value}" STREQUAL "${_target_orig}")
          message(FATAL_ERROR "Something went horribly wrong")
        endif()
      elseif("${_prop_name}" STREQUAL "SOURCES")
        # Create the actual target
        list(REMOVE_ITEM _all_properties "${_prop_name}")
        get_target_property(_prop_value ${_target_orig} ${_prop_name})
        # Icons should be added to the executable
        add_executable(${_target_dest} "${_prop_value}" "${_DADB_APP_ICON}")
      elseif("${_prop_name}" MATCHES "^(ALIASED_TARGET)")
        # Target properties that should not be copied
        list(REMOVE_ITEM _all_properties "${_prop_name}")
      elseif("${_prop_name}" MATCHES "<.+>")
        # Remove other properties containing "<...>" that are not target properties.
        list(REMOVE_ITEM _all_properties "${_prop_name}")
      endif()
    endforeach()

    # Now cycle over the cleaned up properties and set all the properties on
    # the target.
    foreach(_prop_name ${_all_properties})
      get_property(_prop_set TARGET ${_target_orig} PROPERTY ${_prop_name} SET)
      if(_prop_set)
        get_property(_prop_value TARGET ${_target_orig} PROPERTY ${_prop_name})
        # Some properties (i.e. ALIASED_TARGET) are always SET to NOTFOUND, but
        # we don't want to print them, hence we filter them out.
        # See also: https://cmake.org/Bug/view.php?id=15783
        if(NOT "${_prop_value}" STREQUAL "_prop_value-NOTFOUND")
          set_target_properties(${_target_dest} PROPERTIES "${_prop_name}" "${_prop_value}")
        endif()
      endif()
    endforeach()

    # Enable bundle creation
    set_target_properties(${_target_dest} PROPERTIES MACOSX_BUNDLE ON)

    # Set icon for the bundle
    if (DEFINED _DADB_APP_ICON)
      get_filename_component(_filename "${_DADB_APP_ICON}" NAME)
      set_target_properties(${_target_dest} PROPERTIES MACOSX_BUNDLE_ICON_FILE "${_filename}")
      set_source_files_properties(${_DADB_APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    endif()

    # Update RPATH
    if(NOT CMAKE_SKIP_RPATH AND NOT CMAKE_SKIP_INSTALL_RPATH)
      file(RELATIVE_PATH _rel_path "${CMAKE_INSTALL_FULL_BINDIR}/${_target_dest}/Contents/MacOS/" "${CMAKE_INSTALL_FULL_LIBDIR}")
      if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        get_target_property(CURRENT_RPATH "${_target_dest}" INSTALL_RPATH)
        list(APPEND CURRENT_RPATH "@loader_path/${_rel_path}")
        set_target_properties("${_target_dest}" PROPERTIES INSTALL_RPATH "${CURRENT_RPATH}")
        get_target_property(CURRENT_RPATH "${_target_dest}" INSTALL_RPATH)
      endif()
    endif()

    if(DEFINED _DADB_INSTALL_DESTINATION)
      unset(_component)
      if(DEFINED _DADB_INSTALL_COMPONENT)
        set(_component COMPONENT "${_DADB_INSTALL_COMPONENT}")
      endif()
      install(TARGETS ${_target_dest} ${_component} DESTINATION "${_DADB_INSTALL_DESTINATION}")
    endif()

 endif()

endfunction()
