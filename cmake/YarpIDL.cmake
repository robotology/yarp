# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

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
#                   [THRIFT_INCLUDE_PREFIX]
#                   [THRIFT_NO_NAMESPACE_PREFIX]
#                   [THRIFT_NO_COPYRIGHT}
#                   [THRIFT_NO_EDITOR]
#                   [THRIFT_NO_DOC]
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
#   add_executable(foo main.cpp ${THRIFT_GEN_FILES})


# Avoid multiple inclusions of this file
if(COMMAND _yarp_idl_rosmsg_to_file_list)
  return()
endif()

function(YARP_IDL_TO_DIR)

  set(_options VERBOSE
               THRIFT_INCLUDE_PREFIX
               THRIFT_NO_NAMESPACE_PREFIX
               THRIFT_NO_COPYRIGHT
               THRIFT_NO_EDITOR
               THRIFT_NO_DOC)
  set(_oneValueArgs OUTPUT_DIR
                    SOURCES_VAR
                    HEADERS_VAR
                    INCLUDE_DIRS_VAR)
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
    get_filename_component(_include_prefix ${_file} PATH)
    string(REPLACE "../" "" _include_prefix "${_include_prefix}")
    get_filename_component(_name ${_file} NAME_WE)
    get_filename_component(_ext ${_file} EXT)
    string(TOLOWER "${_ext}" _ext)
    string(TOLOWER "${_name}" _name_lower)
    set(_dir_add "")

    # Figure out format we are working with.
    set(_yarpidl_native 0)
    if(_ext STREQUAL ".thrift")
      set(_family thrift)
      set(_dir_add "/${_name_lower}")
    elseif("${_ext}" MATCHES "\\.(msg|srv)" OR
           "${_file}" MATCHES "^(time|duration)$")
      set(_family rosmsg)
      if("${_file}" STREQUAL "time")
        set(_name TickTime)
        set(_yarpidl_native 1)
      elseif("${_file}" STREQUAL "duration")
        set(_name TickDuration)
        set(_yarpidl_native 1)
      endif()
    else()
      message(FATAL_ERROR "yarp_idl_to_dir does not know what to do with \"${_file}\", unrecognized extension \"${_ext}\"")
    endif()

    if("${_family}" STREQUAL "thrift")
      set(_target_name "${_file}")
    else()
      get_filename_component(_rospkg_name "${_include_prefix}" NAME)
      get_filename_component(_include_prefix "${_include_prefix}" PATH)
      if(_rospkg_name MATCHES "(msg|srv)")
        get_filename_component(_rospkg_name "${_include_prefix}" NAME)
        get_filename_component(_include_prefix "${_include_prefix}" PATH)
      endif()
      if(NOT "${_rospkg_name}" STREQUAL "")
        set(_target_name "${_rospkg_name}_${_name}${_ext}")
      else()
        set(_target_name "${_name}${_ext}")
      endif()
    endif()
    string(REGEX REPLACE "[^a-zA-Z0-9]" "_" _target_name ${_target_name})

    string(LENGTH "${_include_prefix}" _include_prefix_len)
    if(_include_prefix_len GREATER 0)
      set(_include_prefix "/${_include_prefix}")
    endif()

    # Set intermediate output directory.
    set(_dir ${CMAKE_CURRENT_BINARY_DIR}/_yarp_idl_${_include_prefix}${_dir_add})

    # Set cmake script file name
    set(_cmake_script "${_YITD_OUTPUT_DIR}/${_target_name}.cmake")

    # Ensure that the intermediate output directory is deleted on make clean
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${_dir})

    # Check if generation has never happened.
    set(files_missing TRUE)
    if(EXISTS ${_cmake_script})
      set(files_missing FALSE)
    endif()

    # Flag to control whether IDL generation is allowed.
    option(ALLOW_IDL_GENERATION "Allow YARP to (re)build IDL files as needed" ${files_missing})

    if(ALLOW_IDL_GENERATION)
      # Say what we are doing.
      message(STATUS "${_family} code for ${_file} => ${_YITD_OUTPUT_DIR}")
      # Generate code at configuration time, so we know filenames.
      find_program(YARPIDL_${_family}_LOCATION
                   NAMES yarpidl_${_family}
                   HINTS ${YARP_IDL_BINARY_HINT} # This is a list of directories defined
                                                 # in YarpOptions.cmake (for YARP) or in
                                                 # YARPConfig.cmake for dependencies
                   NO_DEFAULT_PATH)
      # Make sure intermediate output directory exists.
      file(MAKE_DIRECTORY "${_dir}")
      # Generate a script controlling final layout of files.
      # Make sure that variables are visible when expanding templates.
      set(_output_dir ${_YITD_OUTPUT_DIR})
      configure_file("${YARP_MODULE_DIR}/template/placeGeneratedYarpIdlFiles.cmake.in"
                     "${_dir}/place${_name}.cmake"
                     @ONLY)
      unset(_args)
      if("${_family}" STREQUAL "thrift")
        unset(_extra_args)
        foreach(_ARG INCLUDE_PREFIX
                     NO_NAMESPACE_PREFIX
                     NO_COPYRIGHT
                     NO_EDITOR
                     NO_DOC)
          if(_YITD_THRIFT_${_ARG})
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
      else()
        list(APPEND _args --no-ros true)
        list(APPEND _args --no-cache)
        if(_YITD_VERBOSE)
          list(APPEND _args --verbose)
        endif()
      endif()
      list(APPEND _args --out "${_dir}")

      set(_output_quiet OUTPUT_QUIET)
      if(_YITD_VERBOSE)
        set(_output_quiet)
      endif()

      set(_cmd "${YARPIDL_${_family}_LOCATION}" ${_args} "${_file}")

      if(_YITD_VERBOSE)
        string(REPLACE ";" " " _cmd_str "${_cmd}")
        message(STATUS "    WORKING_DIRECTORY = [${CMAKE_CURRENT_SOURCE_DIR}]")
        message(STATUS "    COMMAND = [${_cmd_str}]")
      endif()

      # Go ahead and generate files.
      execute_process(COMMAND ${_cmd}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                      RESULT_VARIABLE res
                      ${_output_quiet})
      # Failure is bad news, let user know.
      if(NOT "${res}" STREQUAL "0")
        message(FATAL_ERROR "yarpidl_${_family} (${YARPIDL_${_family}_LOCATION}) failed, aborting.")
      endif()
      # Place the files in their final location.
      execute_process(COMMAND ${CMAKE_COMMAND} -P "${_dir}/place${_name}.cmake")
      set(files_missing FALSE)
    endif()

    # Prepare list of generated files.
    if(NOT files_missing)
      include(${_cmake_script})
      set(DEST_FILES)
      foreach(generatedFile ${headers})
        list(APPEND DEST_FILES ${_YITD_OUTPUT_DIR}/${generatedFile})
        list(APPEND _full_headers ${_YITD_OUTPUT_DIR}/${generatedFile})
      endforeach(generatedFile)
      foreach(generatedFile ${sources})
        list(APPEND DEST_FILES ${_YITD_OUTPUT_DIR}/${generatedFile})
        list(APPEND _full_sources ${_YITD_OUTPUT_DIR}/${generatedFile})
      endforeach(generatedFile)
    endif()

    if(ALLOW_IDL_GENERATION)
      # Add a command/target to regenerate the files if the IDL file changes.
      set(yarpidl_depends ${_file})
      if(_yarpidl_native)
        unset(yarpidl_depends)
      endif()
      add_custom_command(OUTPUT ${_YITD_OUTPUT_DIR}/${_target_name}.cmake ${DEST_FILES}
                         COMMAND ${YARPIDL_${_family}_LOCATION} --out ${_dir} --gen yarp:include_prefix --I ${CMAKE_CURRENT_SOURCE_DIR} ${_file}
                         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                         COMMAND ${CMAKE_COMMAND} -P ${_dir}/place${_name}.cmake
                         DEPENDS ${yarpidl_depends} ${YARPIDL_LOCATION})
      add_custom_target(${_target_name} DEPENDS ${_YITD_OUTPUT_DIR}/${_target_name}.cmake)

      # Put the target in the right folder if defined
      get_property(autogen_source_group_set GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER SET)
      if(autogen_source_group_set)
        get_property(autogen_targets_folder GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER)
        set_property(TARGET ${_target_name} PROPERTY FOLDER "${autogen_targets_folder}")
      endif()
    else()
      if(files_missing)
        message(FATAL_ERROR "Generated IDL files for ${_file} not found and cannot make them because ALLOW_IDL_GENERATION=${ALLOW_IDL_GENERATION} (maybe this should be turned on?)")
      endif()
    endif(ALLOW_IDL_GENERATION)

    list(APPEND _full_paths "${_YITD_OUTPUT_DIR}/include")
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
  # FIXME for now namespace_dir is not used, enable when yarp_add_idl will be
  #       able to handle namespaces
  unset(namespace_dir)

  # Match "enum"s, "struct"s and "service"s defined in the file
  string(REGEX MATCHALL "(enum|struct|service)[ \t\n]+([^ \t\n]+)[ \t\n]*{[^}]+}([ \t\n]*\\([^\\)]+\\))?" objects ${file_content})

  # Find object name and append generated files
  foreach(object ${objects})
    string(REGEX MATCH "^(enum|struct|service)[ \t\n]+([^ \t\n{]+)" unused ${object})
    set(objectname ${CMAKE_MATCH_2})
    if(NOT "${object}" MATCHES "{[^}]+}[ \t\n]*(\\([^\\)]*yarp.name[^\\)]+\\))")
      # No files are generated for YARP types.
      list(APPEND gen_srcs ${objectname}.cpp)
      list(APPEND gen_hdrs ${path}/${namespace_dir}/${objectname}.h)
    endif()
  endforeach()

  # Remove "enum"s, "struct"s and "service"s
  string(REGEX REPLACE "(enum|struct|service)[ \t\n]+([^ \t\n]+)[ \t\n]*{[^}]+}([ \t\n]*\\([^\\)]+\\))?" "" file_content ${file_content})

  # Find if at least one "const" or "typedef" is defined
  if("${file_content}" MATCHES "(const|typedef)[ \t]+([^ \t\n]+)[ \t]*([^ \t\n]+)")
    list(APPEND gen_hdrs ${path}/${namespace_dir}/${basename}_common.h)
  endif()

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

  set(${gen_srcs_var} ${gen_srcs} PARENT_SCOPE)
  set(${gen_hdrs_var} ${gen_hdrs} PARENT_SCOPE)
endfunction()


function(YARP_ADD_IDL var)

  # Ensure that the output variable is empty
  unset(${var})
  unset(include_dirs)

  set(_files ${ARGN})
  foreach(file ${_files})

    # Ensure that the filename is relative to the current source directory
    if(IS_ABSOLUTE "${file}")
      file(RELATIVE_PATH file "${CMAKE_CURRENT_SOURCE_DIR}" "${file}")
    endif()

    # Extract a name and extension.
    get_filename_component(path ${file} PATH)
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
      get_filename_component(path "${path}" PATH)
      if(pkg MATCHES "(msg|srv)")
        get_filename_component(pkg "${path}" NAME)
        get_filename_component(path "${path}" PATH)
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
      message(FATAL_ERROR "Unknown extension ${ext}. Supported extensiona are .thrift, .msg, and .srv")
    endif()

    # FIXME This should handle cross-compiling
    set(YARPIDL_${family}_COMMAND YARP::yarpidl_${family})

    # Set intermediate output directory, remove extra '/' and ensure that
    # the directory exists.
    set(tmp_dir "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/yarpidl_${family}")
    string(REGEX REPLACE "/(/|$)" "\\1" tmp_dir "${tmp_dir}")
    file(MAKE_DIRECTORY "${tmp_dir}")

    # Set output directories and remove extra "/"
    set(srcs_out_dir "${CMAKE_CURRENT_BINARY_DIR}/src")
    set(hdrs_out_dir "${CMAKE_CURRENT_BINARY_DIR}/include")
    string(REGEX REPLACE "/(/|$)" "\\1" srcs_out_dir "${srcs_out_dir}")
    string(REGEX REPLACE "/(/|$)" "\\1" hdrs_out_dir "${hdrs_out_dir}")

    # Prepare main command
    if("${family}" STREQUAL "thrift")
      set(cmd ${YARPIDL_thrift_COMMAND} --gen yarp:include_prefix,no_namespace_prefix --I "${CMAKE_CURRENT_SOURCE_DIR}" --out "${tmp_dir}" "${file}")
    else()
      if(YARPIDL_rosmsg_VERBOSE)
        set(_verbose --verbose)
      endif()
      set(cmd ${YARPIDL_rosmsg_COMMAND} --no-ros true --no-cache --no-index ${_verbose} --out "${CMAKE_CURRENT_BINARY_DIR}/include" "${file}")
    endif()

    # Prepare copy command (thrift only) and populate output variable
    unset(output)
    foreach(gen_file ${gen_srcs})
      set(out "${srcs_out_dir}/${gen_file}")
      list(FIND ${var} ${out} x)
      if(x EQUAL -1)
        list(APPEND output "${out}")
        if("${family}" STREQUAL "thrift")
          set(in "${tmp_dir}/${gen_file}")
          list(APPEND cmd COMMAND ${CMAKE_COMMAND} -E copy "${in}" "${out}")
        endif()
      endif()
    endforeach()
    foreach(gen_file ${gen_hdrs})
      if("${family}" STREQUAL "thrift")
        set(out "${hdrs_out_dir}/${path}/${gen_file}")
      else()
        set(out "${hdrs_out_dir}/${gen_file}")
      endif()
      list(FIND ${var} ${out} x)
      if(x EQUAL -1)
        list(APPEND output "${out}")
        if("${family}" STREQUAL "thrift")
          set(in "${tmp_dir}/${gen_file}")
          list(APPEND cmd COMMAND ${CMAKE_COMMAND} -E copy "${in}" "${out}")
        endif()
      endif()
    endforeach()

    if(NOT "${output}" STREQUAL "")
      set(depends ${YARPIDL_${family}_COMMAND})
      if(NOT native)
        list(APPEND depends ${file})
      endif()
      add_custom_command(OUTPUT ${output}
                         DEPENDS ${depends}
                         COMMAND ${cmd}
                         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                         COMMENT "Generating code from ${file}")

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
  include_directories("${CMAKE_CURRENT_BINARY_DIR}/include/")

endfunction()
