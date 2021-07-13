# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# yarp_idl_to_dir
# ---------------
#
# Take an IDL file and generate code for it in the specified directory,
# optionally storing the list of source/header files in the supplied
# variables. Call as:
#
#   yarp_idl_to_dir(INPUT_FILES <file> [file]
#                   OUTPUT_DIR <dir>
#                   [SOURCES_VAR <var>]
#                   [HEADERS_VAR <var>]
#                   [INCLUDE_DIRS_VAR <var>]
#                   [PLACEMENT <MERGED|SEPARATE|SEPARATE_EVEN|SEPARATE_ODD>
#                       MERGED:   headers and sources in <OUTPUT_DIR>/<namespace>
#                       SEPARATE: headers in <OUTPUT_DIR>/include/<namespace> sources in <OUTPUT_DIR>/src/<namespace>
#                       SEPARATE_EVEN: Alias for SEPARATE
#                       SEPARATE_ODD:  headers in <OUTPUT_DIR>/include/<namespace> sources in <OUTPUT_DIR>/src
#                   [THRIFT_INCLUDE_PREFIX]
#                   [THRIFT_NO_NAMESPACE_PREFIX]
#                   [THRIFT_NO_COPYRIGHT}
#                   [THRIFT_NO_EDITOR]
#                   [THRIFT_NO_DOC]
#                   [THRIFT_DEBUG_GENERATOR]
#                   [ROSMSG_WITH_ROS]
#                   [VERBOSE])
#
# yarp_add_idl
# ------------
#
# Take one or more IDL files and generate code at build time.
# Files will be regenerated whenever the IDL file changes.
#
#   yarp_add_idl(<var> <file> [file [...]])
#
# The <var> variable, will contain the generated files, and can be
# added to the an add_executable or add_library call. For example:
#
#   set(THRIFT_FILES file1.thrift
#                    file2.msg
#                    file3.srv)
#   yarp_add_idl(THRIFT_GEN_FILES ${THRIFT_FILES})
#   add_executable(foo)
#   target_sources(foo PRIVATE main.cpp ${THRIFT_GEN_FILES})
#
# The ``YARP_ADD_IDL_INCLUDE_DIR`` variable contains the include directory for
# using the header files


# Avoid multiple inclusions of this file
if(COMMAND _yarp_idl_rosmsg_to_file_list)
  return()
endif()

# Internal function.
function(_YARP_IDL_THRIFT_ARGS _prefix _out_dir _verbose _out_var)
  unset(_args)
  unset(_extra_args)
  foreach(_ARG
    INCLUDE_PREFIX
    NO_NAMESPACE_PREFIX
    NO_COPYRIGHT
    NO_EDITOR
    NO_DOC
    DEBUG_GENERATOR
  )
    if(${_prefix}_THRIFT_${_ARG})
      string(TOLOWER "${_ARG}" _arg)
      list(APPEND _extra_args ${_arg})
    endif()
  endforeach()
  if (NOT "${_extra_args}" STREQUAL "")
    string(REPLACE ";" "," _extra_args "${_extra_args}")
    set(_extra_args ":${_extra_args}")
  endif()
  list(APPEND _args --gen yarp${_extra_args})
  list(APPEND _args --I "${CMAKE_CURRENT_SOURCE_DIR}")
  if(_verbose)
    list(APPEND _args -v)
  endif()
  list(APPEND _args --out "${_out_dir}")

  set(${_out_var} ${_args} PARENT_SCOPE)
endfunction()

# Internal function.
function(_YARP_IDL_ROSMSG_ARGS _prefix _out_dir _verbose _out_var)
  unset(_args)
  if(NOT ${_prefix}_ROSMSG_WITH_ROS)
    list(APPEND _args --no-ros true)
  endif()
  list(APPEND _args --no-cache)
  if(_verbose)
    list(APPEND _args --verbose)
  endif()
  list(APPEND _args --out "${_out_dir}")

  set(${_out_var} ${_args} PARENT_SCOPE)
endfunction()


# Internal function.
# Perform the actual code generation
function(_YARP_IDL_TO_DIR_GENERATE _family _file _name _index_file_name _output_dir _include_prefix _cpp_placement_dir _h_placement_dir _cpp_placement_ns _verbose)
  # Say what we are doing.
  message(STATUS "${_family} code for ${_file} => ${_output_dir}")

  # Set intermediate output directory.
  set(_temp_dir "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/yarp_idl_to_dir")
  if(NOT "${_include_prefix}" STREQUAL "")
    set(_temp_dir "${_temp_dir}/${_include_prefix}")
  endif()

  # Make sure intermediate output directory exists.
  file(MAKE_DIRECTORY "${_temp_dir}")

  # Ensure that the intermediate output directory is deleted on make clean
  set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${_temp_dir})

  # Generate code at configuration time, so we know filenames.
  find_program(YARPIDL_${_family}_LOCATION
    NAMES yarpidl_${_family}
    HINTS ${YARP_IDL_BINARY_HINT} # This is a list of directories defined
                                  # in YarpOptions.cmake (for YARP) or in
                                  # YARPConfig.cmake for dependencies
    NO_DEFAULT_PATH
  )

  if("${_family}" STREQUAL "thrift")
    _yarp_idl_thrift_args(_YITD "${_temp_dir}" ${_verbose} _args)
  else()
    _yarp_idl_rosmsg_args(_YITD "${_temp_dir}" ${_verbose} _args)
  endif()

  set(_output_quiet OUTPUT_QUIET)
  if(_verbose)
    set(_output_quiet)
  endif()

  set(_cmd "${YARPIDL_${_family}_LOCATION}" ${_args} "${_file}")

  if(_verbose)
    string(REPLACE ";" " " _cmd_str "${_cmd}")
    message(STATUS "    WORKING_DIRECTORY = [${CMAKE_CURRENT_SOURCE_DIR}]")
    message(STATUS "    COMMAND = [${_cmd_str}]")
  endif()

  # Go ahead and generate files.
  execute_process(
    COMMAND ${_cmd}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE res
    ${_output_quiet}
  )
  # Failure is bad news, let user know.
  if(NOT "${res}" STREQUAL "0")
    message(FATAL_ERROR "yarpidl_${_family} (${YARPIDL_${_family}_LOCATION}) failed, aborting.")
  endif()

  # Place the files in their final location.
  file(STRINGS "${_temp_dir}/${_index_file_name}" _index)
  file(WRITE "${_output_dir}/${_index_file_name}" "")
  foreach(_gen_file ${_index})
    if(NOT EXISTS "${_temp_dir}/${_gen_file}")
      message(FATAL_ERROR "${_gen_file} not found in ${_temp_dir} dir!")
    endif()
    get_filename_component(_type ${_gen_file} EXT)
    if(${_type} STREQUAL ".h")
      set(_dest_dir "${_h_placement_dir}")
      set(_dest_file "${_dest_dir}/${_gen_file}")
    elseif(${_type} STREQUAL ".cpp")
      set(_dest_dir "${_cpp_placement_dir}")
      set(_dest_file "${_gen_file}")
      if(NOT _cpp_placement_ns)
        string(REGEX REPLACE "^.+/([^/]+)$" "\\1" _dest_file "${_dest_file}")
      endif()
      set(_dest_file "${_dest_dir}/${_dest_file}")
    else()
      message(FATAL_ERROR "Filename extension of ${_gen_file} is neither .h nor .cpp")
    endif()
    string(REGEX REPLACE "^/+" "" _dest_file "${_dest_file}")

    file(APPEND "${_output_dir}/${_index_file_name}" "${_dest_file}\n")
    configure_file(
      "${_temp_dir}/${_gen_file}"
      "${_output_dir}/${_dest_file}"
      COPYONLY
    )
  endforeach()
endfunction()


function(YARP_IDL_TO_DIR)

  # Flag to control whether IDL generation is allowed.
  option(ALLOW_IDL_GENERATION "Allow YARP to (re)build IDL files as needed" OFF)

  set(_options
    VERBOSE
    THRIFT_INCLUDE_PREFIX
    THRIFT_NO_NAMESPACE_PREFIX
    THRIFT_NO_COPYRIGHT
    THRIFT_NO_EDITOR
    THRIFT_NO_DOC
    THRIFT_DEBUG_GENERATOR
    ROSMSG_WITH_ROS
  )
  set(_oneValueArgs
    OUTPUT_DIR
    SOURCES_VAR
    HEADERS_VAR
    INCLUDE_DIRS_VAR
    PLACEMENT
  )
  set(_multiValueArgs INPUT_FILES)
  cmake_parse_arguments(_YITD "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" ${ARGN} )

  if(NOT YARP_NO_DEPRECATED) # Since YARP 3.2
    # Support old way to pass SOURCES_VAR, HEADERS_VAR and INCLUDE_DIRS_VAR
    # (i.e. using ARGN)
    list(LENGTH _YITD_UNPARSED_ARGUMENTS _len)
    if(_len GREATER 0)
      message(DEPRECATION "Passing variables without name argument to YARP_IDL_TO_DIR is deprecated. Use SOURCES_VAR, HEADERS_VAR and INCLUDE_DIRS_VAR")
      if(NOT _YITD_INPUT_FILES)
        list(GET _YITD_UNPARSED_ARGUMENTS 0 _YITD_INPUT_FILES)
      endif()
      if(NOT _YITD_OUTPUT_DIR)
        list(GET _YITD_UNPARSED_ARGUMENTS 1 _YITD_OUTPUT_DIR)
      endif()
      # include_prefix was always enabled in the old version
      if(NOT _YITD_THRIFT_INCLUDE_PREFIX)
        set(_YITD_THRIFT_INCLUDE_PREFIX 1)
      endif()
      if(NOT _YITD_THRIFT_NO_NAMESPACE_PREFIX)
        set(_YITD_THRIFT_NO_NAMESPACE_PREFIX 1)
      endif()
    endif()
    if(_len GREATER 3)
      if(NOT DEFINED _YITD_SOURCES_VAR)
        list(GET _YITD_UNPARSED_ARGUMENTS 2 _YITD_SOURCES_VAR)
      endif()
      if(NOT DEFINED _YITD_HEADERS_VAR)
        list(GET _YITD_UNPARSED_ARGUMENTS 3 _YITD_HEADERS_VAR)
      endif()
    endif()
    if(_len GREATER 4)
      if(NOT DEFINED _YITD_INCLUDE_DIRS_VAR)
        list(GET _YITD_UNPARSED_ARGUMENTS 4 _YITD_INCLUDE_DIRS_VAR)
      endif()
    endif()
  endif()

  if(NOT DEFINED _YITD_INPUT_FILES)
    message(FATAL_ERROR "Missing INPUT_FILES argument")
  endif()

  if(NOT DEFINED _YITD_OUTPUT_DIR)
    message(FATAL_ERROR "Missing OUTPUT_DIR argument")
  endif()

  if(NOT DEFINED _YITD_PLACEMENT)
    # For now keep the old placement style
    set(_YITD_PLACEMENT SEPARATE_ODD)
  else()
    if(NOT "${_YITD_PLACEMENT}" MATCHES "^(MERGED|SEPARATE|SEPARATE_EVEN|SEPARATE_ODD)$")
      message(FATAL_ERROR "PLACEMENT should be one of MERGED, SEPARATE, SEPARATE_EVEN or SEPARATE_ODD")
    endif()
  endif()

  if("${_YITD_PLACEMENT}" STREQUAL "MERGED")
    set(_cpp_placement_dir "")
    set(_h_placement_dir "")
    set(_cpp_placement_ns 1)
  elseif("${_YITD_PLACEMENT}" MATCHES "^(SEPARATE|SEPARATE_EVEN)$")
    set(_cpp_placement_dir "src")
    set(_h_placement_dir "include")
    set(_cpp_placement_ns 1)
  elseif("${_YITD_PLACEMENT}" STREQUAL "SEPARATE_ODD")
    set(_cpp_placement_dir "src")
    set(_h_placement_dir "include")
    set(_cpp_placement_ns 0)
  endif()

  unset(_full_headers)
  unset(_full_sources)
  unset(_full_paths)

  foreach(_file ${_YITD_INPUT_FILES})

    # Extract a name and extension.
    if(IS_ABSOLUTE ${_file})
      file(RELATIVE_PATH _file ${CMAKE_CURRENT_SOURCE_DIR} ${_file})
    else()
      set(_file ${_file})
    endif()
    get_filename_component(_include_prefix ${_file} DIRECTORY)
    string(REPLACE "../" "" _include_prefix "${_include_prefix}")
    get_filename_component(_name ${_file} NAME_WE)
    get_filename_component(_ext ${_file} EXT)
    string(TOLOWER "${_ext}" _ext)
    string(TOLOWER "${_name}" _name_lower)
    set(_dir_add "")

    # Figure out format we are working with.
    if(_ext STREQUAL ".thrift")
      set(_family thrift)
      set(_dir_add "${_name_lower}")
    elseif("${_ext}" MATCHES "\\.(msg|srv)" OR
           "${_file}" MATCHES "^(time|duration)$")
      set(_family rosmsg)
      if("${_file}" STREQUAL "time")
        set(_name TickTime)
      elseif("${_file}" STREQUAL "duration")
        set(_name TickDuration)
      endif()
    else()
      message(FATAL_ERROR "yarp_idl_to_dir does not know what to do with \"${_file}\", unrecognized extension \"${_ext}\"")
    endif()

    if("${_family}" STREQUAL "thrift")
      set(_target_name "${_file}")
      set(_index_file_name "${_name}_index.txt")
    elseif("${_family}" STREQUAL "rosmsg")
      get_filename_component(_rospkg_name "${_include_prefix}" NAME)
      get_filename_component(_include_prefix "${_include_prefix}" DIRECTORY)
      if(_rospkg_name MATCHES "^(msg|srv)$")
        get_filename_component(_rospkg_name "${_include_prefix}" NAME)
        get_filename_component(_include_prefix "${_include_prefix}" DIRECTORY)
      endif()
      if(NOT "${_rospkg_name}" STREQUAL "")
        set(_target_name "${_rospkg_name}_${_name}${_ext}")
        set(_index_file_name "${_rospkg_name}_${_name}_index.txt")
      else()
        set(_target_name "${_name}${_ext}")
        set(_index_file_name "${_name}_index.txt")
      endif()
    endif()
    string(REGEX REPLACE "[^a-zA-Z0-9]" "_" _target_name ${_target_name})
    set(_index_file "${_YITD_OUTPUT_DIR}/${_index_file_name}")

    set(_include_prefix "${_include_prefix}/${_dir_add}")
    string(REGEX REPLACE "^/+" "" _include_prefix "${_include_prefix}")
    string(REGEX REPLACE "/+$" "" _include_prefix "${_include_prefix}")

    if(ALLOW_IDL_GENERATION OR NOT EXISTS "${_index_file}")
      _yarp_idl_to_dir_generate(
        ${_family}
        "${_file}"
        ${_name}
        ${_index_file_name}
        "${_YITD_OUTPUT_DIR}"
        "${_include_prefix}"
        "${_cpp_placement_dir}"
        "${_h_placement_dir}"
        ${_cpp_placement_ns}
        ${_YITD_VERBOSE}
      )
    endif()

    # Sanity check, it should never happen
    if(NOT EXISTS "${_index_file}")
      message(FATAL_ERROR "Something went wrong in idl generation. Missing \"${_index_file}\" file")
    endif()

    # Read index file
    file(STRINGS "${_index_file}" _index)
    foreach(_gen_file ${_index})
      if(NOT EXISTS "${_YITD_OUTPUT_DIR}/${_gen_file}")
        message(FATAL_ERROR "Something went wrong in idl generation. Missing \"${_YITD_OUTPUT_DIR}/${_gen_file}\" file")
      endif()
      file(RELATIVE_PATH _rel_file ${CMAKE_CURRENT_LIST_DIR} "${_YITD_OUTPUT_DIR}/${_gen_file}")
      get_filename_component(_type ${_gen_file} EXT)
      if(${_type} STREQUAL ".h")
        list(APPEND _full_headers "${_rel_file}")
      elseif(${_type} STREQUAL ".cpp")
        list(APPEND _full_sources "${_rel_file}")
      else()
        message(WARNING "Filename extension of ${_gen_file} is neither .h nor .cpp")
      endif()
    endforeach()

    list(APPEND _full_paths "${_YITD_OUTPUT_DIR}/${_h_placement_dir}")
    list(REMOVE_DUPLICATES _full_paths)
  endforeach()

  # Set requested variables in parent scope
  if(DEFINED _YITD_SOURCES_VAR)
    set(${_YITD_SOURCES_VAR} ${_full_sources} PARENT_SCOPE)
  endif()
  if(DEFINED _YITD_HEADERS_VAR)
    set(${_YITD_HEADERS_VAR} ${_full_headers} PARENT_SCOPE)
  endif()
  if(DEFINED _YITD_INCLUDE_DIRS_VAR)
    set(${_YITD_INCLUDE_DIRS_VAR} ${_full_paths} PARENT_SCOPE)
  endif()

endfunction()


# Internal function.
# Calculate a list of sources generated from a .thrift file
function(_YARP_IDL_THRIFT_TO_FILE_LIST file path basename ext gen_srcs_var gen_hdrs_var)
  set(gen_srcs)
  set(gen_hdrs)

  # Read thrift file
  file(READ ${file} file_content)

  # Remove comments
  string(REGEX REPLACE "/\\*[^*]?[^/]+\\*/" "" file_content ${file_content})
  string(REGEX REPLACE "#[^\n]+" "" file_content ${file_content})
  string(REGEX REPLACE "//[^\n]+" "" file_content ${file_content})

  # Match "namespace"
  string(REGEX MATCH "namespace[ \t\n]+yarp[ \t\n]+([^ \t\n]+)" _unused ${file_content})
  string(REPLACE "." "/" namespace_dir "${CMAKE_MATCH_1}")

  if(NOT YARP_ADD_IDL_THRIFT_INCLUDE_PREFIX)
    set(path)
  endif()

  if(YARP_ADD_IDL_THRIFT_NO_NAMESPACE_PREFIX)
    set(namespace_dir)
  endif()

  # Match "enum"s, "struct"s and "service"s defined in the file
  string(REGEX MATCHALL "(enum|struct|service)[ \t\n]+([^ \t\n]+)[ \t\n]*{[^}]+}([ \t\n]*\\([^\\)]+\\))?" objects ${file_content})

  # Find object name and append generated files
  foreach(object ${objects})
    string(REGEX MATCH "^(enum|struct|service)[ \t\n]+([^ \t\n{]+)" unused ${object})
    set(objectname ${CMAKE_MATCH_2})
    if(NOT "${object}" MATCHES "{[^}]+}[ \t\n]*(\\([^\\)]*yarp.name[^\\)]+\\))")
      # No files are generated for YARP types.
      set(file_path "${path}/${namespace_dir}")
      string(REGEX REPLACE "^/+" "" file_path "${file_path}")
      list(APPEND gen_srcs "${file_path}/${objectname}.cpp")
      list(APPEND gen_hdrs "${file_path}/${objectname}.h")
    endif()
  endforeach()

  # Remove "enum"s, "struct"s and "service"s
  string(REGEX REPLACE "(enum|struct|service)[ \t\n]+([^ \t\n]+)[ \t\n]*{[^}]+}([ \t\n]*\\([^\\)]+\\))?" "" file_content ${file_content})

  # Find if at least one "const" or "typedef" is defined
  if("${file_content}" MATCHES "(const|typedef)[ \t]+([^ \t\n]+)[ \t]*([^ \t\n]+)")
    list(APPEND gen_hdrs ${path}/${namespace_dir}/${basename}_common.h)
  endif()

  string(REGEX REPLACE "/(/|$)" "\\1" gen_srcs "${gen_srcs}")
  string(REGEX REPLACE "/(/|$)" "\\1" gen_hdrs "${gen_hdrs}")

  set(${gen_srcs_var} ${gen_srcs} PARENT_SCOPE)
  set(${gen_hdrs_var} ${gen_hdrs} PARENT_SCOPE)
endfunction()


# Internal function.
# Calculate a list of sources generated from a .msg or a .srv file
function(_YARP_IDL_ROSMSG_TO_FILE_LIST file path pkg basename ext gen_srcs_var gen_hdrs_var)
  set(gen_srcs )
  set(gen_hdrs )

  get_filename_component(ext ${file} EXT)

  unset(gen_hdrs)
  if(NOT "${pkg}" STREQUAL "")
    list(APPEND gen_hdrs "yarp/rosmsg/${pkg}/${basename}.h")
  else()
    list(APPEND gen_hdrs "yarp/rosmsg/${basename}.h")
  endif()

  if(NOT YARP_NO_DEPRECATED)
    if(NOT "${pkg}" STREQUAL "")
      list(APPEND gen_hdrs "${pkg}/${basename}.h"
                           "${pkg}_${basename}.h")
    else()
      list(APPEND gen_hdrs "${basename}.h")
    endif()
  endif()

  if("${ext}" STREQUAL ".srv")
    if(NOT "${pkg}" STREQUAL "")
      list(APPEND gen_hdrs "yarp/rosmsg/${pkg}/${basename}Reply.h")
    else()
      list(APPEND gen_hdrs "yarp/rosmsg/${basename}Reply.h")
    endif()

    if(NOT YARP_NO_DEPRECATED)
      if(NOT "${pkg}" STREQUAL "")
        list(APPEND gen_hdrs "${pkg}/${basename}Reply.h"
                             "${pkg}_${basename}Reply.h")
      else()
        list(APPEND gen_hdrs "${basename}Reply.h")
      endif()
    endif()
  endif()

  string(REGEX REPLACE "/(/|$)" "\\1" gen_srcs "${gen_srcs}")
  string(REGEX REPLACE "/(/|$)" "\\1" gen_hdrs "${gen_hdrs}")

  set(${gen_srcs_var} ${gen_srcs} PARENT_SCOPE)
  set(${gen_hdrs_var} ${gen_hdrs} PARENT_SCOPE)
endfunction()


function(YARP_ADD_IDL var)

  # Ensure that the output variable is empty
  unset(${var})
  unset(include_dirs)

  # Ensure that these 2 variables are true when not defined, in order to be
  # compatible with the original behaviour
  if (NOT DEFINED YARP_ADD_IDL_THRIFT_INCLUDE_PREFIX)
    set(YARP_ADD_IDL_THRIFT_INCLUDE_PREFIX 1)
  endif()
  if (NOT DEFINED YARP_ADD_IDL_THRIFT_NO_NAMESPACE_PREFIX)
    set(YARP_ADD_IDL_THRIFT_NO_NAMESPACE_PREFIX 1)
  endif()

  set(_files ${ARGN})
  foreach(file ${_files})

    # Ensure that the filename is relative to the current source directory
    if(IS_ABSOLUTE "${file}")
      file(RELATIVE_PATH file "${CMAKE_CURRENT_SOURCE_DIR}" "${file}")
    endif()

    # Extract a name and extension.
    get_filename_component(path ${file} DIRECTORY)
    get_filename_component(basename ${file} NAME_WE)
    get_filename_component(ext ${file} EXT)
    string(TOLOWER "${ext}" ext)

    # Figure out format we are working with and determine which files
    # will be generated
    set(native 0)
    if("${ext}" STREQUAL ".thrift")
      set(family thrift)
      _yarp_idl_thrift_to_file_list("${file}" "${path}" "${basename}" ${ext} gen_srcs gen_hdrs)
    elseif("${ext}" MATCHES "^\\.(msg|srv)$")
      set(family rosmsg)
      get_filename_component(pkg "${path}" NAME)
      get_filename_component(path "${path}" DIRECTORY)
      if(pkg MATCHES "(msg|srv)")
        get_filename_component(pkg "${path}" NAME)
        get_filename_component(path "${path}" DIRECTORY)
      endif()
      _yarp_idl_rosmsg_to_file_list("${file}" "${path}" "${pkg}" "${basename}" ${ext} gen_srcs gen_hdrs)
    elseif("${file}" STREQUAL "time")
      set(family rosmsg)
      set(native 1)
      set(gen_hdrs yarp/rosmsg/TickTime.h)
      if(NOT YARP_NO_DEPRECATED)
        list(APPEND gen_hdrs TickTime.h)
      endif()
    elseif("${file}" STREQUAL "duration")
      set(family rosmsg)
      set(native 1)
      set(gen_hdrs yarp/rosmsg/TickDuration.h)
      if(NOT YARP_NO_DEPRECATED)
        list(APPEND gen_hdrs TickDuration.h)
      endif()
    else()
      message(FATAL_ERROR "Unknown extension ${ext}. Supported extensions are .thrift, .msg, and .srv")
    endif()

    # FIXME This should handle cross-compiling
    set(YARPIDL_${family}_COMMAND YARP::yarpidl_${family})

    # Set output directories and remove extra "/" and ensure that the directory
    # exists.
    set(out_dir "${CMAKE_CURRENT_BINARY_DIR}")
    string(REGEX REPLACE "/(/|$)" "\\1" out_dir "${out_dir}")
    file(MAKE_DIRECTORY "${out_dir}")

    set(_verbose 0)
    if(YARPIDL_rosmsg_VERBOSE)
      set(_verbose 1)
    endif()

    if("${family}" STREQUAL "thrift")
      _yarp_idl_thrift_args(YARP_ADD_IDL "${out_dir}" ${_verbose} _args)
    else()
      _yarp_idl_rosmsg_args(YARP_ADD_IDL "${out_dir}" ${_verbose} _args)
    endif()

    # Prepare main command
    if("${family}" STREQUAL "thrift")
      set(cmd ${YARPIDL_thrift_COMMAND} ${_args} "${file}")
    else()
      set(cmd ${YARPIDL_rosmsg_COMMAND} --no-index ${_args} "${file}")
    endif()

    # Populate output variable
    unset(output)
    foreach(gen_file IN LISTS gen_srcs gen_hdrs)
      set(out "${out_dir}/${gen_file}")
      list(FIND ${var} ${out} x)
      if(x EQUAL -1)
        list(APPEND output "${out}")
      endif()
    endforeach()

    if(NOT "${output}" STREQUAL "")
      set(depends ${YARPIDL_${family}_COMMAND})
      if(NOT native)
        list(APPEND depends ${file})
      endif()
      add_custom_command(
        OUTPUT ${output}
        DEPENDS ${depends}
        COMMAND ${cmd}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating code from ${file}"
      )

      # Append output to return variable
      list(APPEND ${var} ${output})

      # Mark the generated files as generated
      set_source_files_properties(${output} PROPERTIES GENERATED TRUE)
      # Put the files in the right source group if defined
      get_property(autogen_source_group_set GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP SET)
      if(autogen_source_group_set)
        get_property(autogen_source_group GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP)
        source_group("${autogen_source_group}" FILES ${output})
      endif()
    endif()

    # Force CMake to run again if the file is modified (not just the build
    # is not enough).
    # This is required because the new version might generate new
    # output files, therefore we need to parse the file again and add the new
    # output files as generated.
    if(NOT ${native})
      set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${file}")
    endif()

  endforeach()

  set(${var} ${${var}} PARENT_SCOPE)
  set(YARP_ADD_IDL_INCLUDE_DIR "${out_dir}" PARENT_SCOPE)

  include_directories("${out_dir}")

endfunction()
