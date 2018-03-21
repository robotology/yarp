# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
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
#   yarp_idl_to_dir(foo.thrift foo [NO_RECURSE])
#   yarp_idl_to_dir(foo.thrift foo [NO_RECURSE] SOURCES HEADERS)
#   yarp_idl_to_dir(foo.thrift foo [NO_RECURSE] SOURCES HEADERS INCLUDE_PATHS)
#
# If the ``NO_RECURSE`` option is set, the --no-recurse option is
# passed to ``yarpidl_rosmsg``, hence generating code only for the
# selected files, not for their dependencies.
#
#
# yarp_add_idl
# ------------
#
# Take one or more IDL files and generate code at build time.
# Files will be regenerated whenever the IDL file changes.
#
#   yarp_add_idl(<var> [NO_RECURSE] <file> [file [...]])
#
# The <var> variable, will contain the generated files, and can be
# added to the an add_executable or add_library call. For example:
#
#   set(THRIFT_FILES file1.thrift
#                    file2.msg
#                    file3.srv)
#   yarp_add_idl(THRIFT_GEN_FILES ${THRIFT_FILES})
#   add_executable(foo main.cpp ${THRIFT_GEN_FILES})
#
# If the ``NO_RECURSE`` option is set, the --no-recurse option is
# passed to ``yarpidl_rosmsg``, hence generating code only for the
# selected files, not for their dependencies.


# Avoid multiple inclusions of this file
if(COMMAND yarp_add_idl)
  return()
endif()

function(YARP_IDL_TO_DIR yarpidl_file_base output_dir)
  # Store optional output variable(s).
  set(out_vars ${ARGN})

  # If the first not named argument (ARGV2) is "NO_RECURSE", it is removed from
  # the out_vars list, and the --no-recurse option is passed to yarpidl_rosmsg
  set(_no_recurse )
  if ("${ARGV2}" STREQUAL "NO_RECURSE")
    list(REMOVE_AT out_vars 0)
    set(_no_recurse --no-recurse)
  endif()

  # Make sure output_dir variable is visible when expanding templates.
  set(output_dir ${output_dir})

  # Extract a name and extension.
  if(IS_ABSOLUTE ${yarpidl_file_base})
    file(RELATIVE_PATH yarpidl_file ${CMAKE_CURRENT_SOURCE_DIR} ${yarpidl_file_base})
  else()
    set(yarpidl_file ${yarpidl_file_base})
  endif()
  get_filename_component(include_prefix ${yarpidl_file} PATH)
  get_filename_component(yarpidlName ${yarpidl_file} NAME_WE)
  get_filename_component(yarpidlExt ${yarpidl_file} EXT)
  string(TOLOWER "${yarpidlExt}" yarpidlExt)
  string(TOLOWER "${yarpidlName}" yarpidlNameLower)
  set(dir_add "")

  # Figure out format we are working with.
  set(yarpidl_native 0)
  if(yarpidlExt STREQUAL ".thrift")
    set(family thrift)
    set(dir_add "/${yarpidlNameLower}")
  elseif("${yarpidlExt}" MATCHES "\\.(msg|srv)" OR
         "${yarpidl_file}" MATCHES "^(time|duration)$")
    set(family rosmsg)
    if("${yarpidl_file}" STREQUAL "time")
      set(yarpidlName TickTime)
      set(yarpidl_native 1)
    elseif("${yarpidl_file}" STREQUAL "duration")
      set(yarpidlName TickDuration)
      set(yarpidl_native 1)
    endif()
  else()
    message(FATAL_ERROR "yarp_idl_to_dir does not know what to do with \"${yarpidl_file}\", unrecognized extension \"${yarpidlExt}\"")
  endif()

  if("${family}" STREQUAL "thrift")
    set(yarpidl_target_name "${yarpidl_file}")
  else()
    get_filename_component(rospkg_name "${include_prefix}" NAME)
    get_filename_component(include_prefix "${include_prefix}" PATH)
    if(rospkg_name MATCHES "(msg|srv)")
      get_filename_component(rospkg_name "${include_prefix}" NAME)
      get_filename_component(include_prefix "${include_prefix}" PATH)
    endif()
    if(NOT "${rospkg_name}" STREQUAL "")
      set(yarpidl_target_name "${rospkg_name}_${yarpidlName}${yarpidlExt}")
    else()
      set(yarpidl_target_name "${yarpidlName}${yarpidlExt}")
    endif()
  endif()
  string(REGEX REPLACE "[^a-zA-Z0-9]" "_" yarpidl_target_name ${yarpidl_target_name})

  string(LENGTH "${include_prefix}" include_prefix_len)
  if(include_prefix_len GREATER 0)
    set(include_prefix "/${include_prefix}")
  endif()

  # Set intermediate output directory.
  set(dir ${CMAKE_CURRENT_BINARY_DIR}/_yarp_idl_${include_prefix}${dir_add})

  # Ensure that the intermediate output directory is deleted on make clean
  set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${dir})

  set(settings_file ${output_dir}/${yarpidl_target_name}.cmake)

  # Check if generation has never happened.
  set(files_missing TRUE)
  if(EXISTS ${settings_file})
    set(files_missing FALSE)
  endif()

  # Flag to control whether IDL generation is allowed.
  option(ALLOW_IDL_GENERATION "Allow YARP to (re)build IDL files as needed" ${files_missing})

  set(full_headers)
  set(full_sources)
  if(ALLOW_IDL_GENERATION)
    # Say what we are doing.
    message(STATUS "${family} code for ${yarpidl_file} => ${output_dir}")
    # Generate code at configuration time, so we know filenames.
    find_program(YARPIDL_${family}_LOCATION
                 NAMES yarpidl_${family}
                 HINTS ${YARP_IDL_BINARY_HINT}
                       "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
    # Make sure intermediate output directory exists.
    make_directory(${dir})
    # Generate a script controlling final layout of files.
    configure_file(${YARP_MODULE_DIR}/template/placeGeneratedYarpIdlFiles.cmake.in ${dir}/place${yarpidlName}.cmake @ONLY)

    if("${family}" STREQUAL "thrift")
      set(cmd ${YARPIDL_thrift_LOCATION} --gen yarp:include_prefix --I "${CMAKE_CURRENT_SOURCE_DIR}" --out "${dir}" "${yarpidl_file}")
    else()
      set(_verbose )
      set(_output_quiet OUTPUT_QUIET)
      if(YARPIDL_rosmsg_VERBOSE)
        set(_verbose --verbose)
        set(_output_quiet )
      endif()
      set(cmd ${YARPIDL_rosmsg_LOCATION} --no-ros true --no-cache ${_verbose} ${_no_recurse} --out "${dir}" "${yarpidl_file}")
    endif()

    # Go ahead and generate files.
    execute_process(COMMAND ${cmd}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE res
                    ${_output_quiet})
    # Failure is bad news, let user know.
    if(NOT "${res}" STREQUAL "0")
      message(FATAL_ERROR "yarpidl_${family} (${YARPIDL_${family}_LOCATION}) failed, aborting.")
    endif()
    # Place the files in their final location.
    execute_process(COMMAND ${CMAKE_COMMAND} -P ${dir}/place${yarpidlName}.cmake)
    set(files_missing FALSE)
  endif()

  # Prepare list of generated files.
  if(NOT files_missing)
    include(${settings_file})
    set(DEST_FILES)
    foreach(generatedFile ${headers})
      list(APPEND DEST_FILES ${output_dir}/${generatedFile})
      list(APPEND full_headers ${output_dir}/${generatedFile})
    endforeach(generatedFile)
    foreach(generatedFile ${sources})
      list(APPEND DEST_FILES ${output_dir}/${generatedFile})
      list(APPEND full_sources ${output_dir}/${generatedFile})
    endforeach(generatedFile)
  endif()

  if(ALLOW_IDL_GENERATION)
    # Add a command/target to regenerate the files if the IDL file changes.
    set(yarpidl_depends ${yarpidl_file})
    if(yarpidl_native)
      unset(yarpidl_depends)
    endif()
    add_custom_command(OUTPUT ${output_dir}/${yarpidl_target_name}.cmake ${DEST_FILES}
                       COMMAND ${YARPIDL_${family}_LOCATION} --out ${dir} --gen yarp:include_prefix --I ${CMAKE_CURRENT_SOURCE_DIR} ${yarpidl_file}
                       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                       COMMAND ${CMAKE_COMMAND} -P ${dir}/place${yarpidlName}.cmake
                       DEPENDS ${yarpidl_depends} ${YARPIDL_LOCATION})
    add_custom_target(${yarpidl_target_name} DEPENDS ${output_dir}/${yarpidl_target_name}.cmake)

    # Put the target in the right folder if defined
    get_property(autogen_source_group_set GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER SET)
    if(autogen_source_group_set)
      get_property(autogen_targets_folder GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER)
      set_property(TARGET ${yarpidl_target_name} PROPERTY FOLDER "${autogen_targets_folder}")
    endif()
  else()
    if(files_missing)
      message(FATAL_ERROR "Generated IDL files for ${yarpidl_file} not found and cannot make them because ALLOW_IDL_GENERATION=${ALLOW_IDL_GENERATION} (maybe this should be turned on?)")
    endif()
  endif(ALLOW_IDL_GENERATION)

  list(LENGTH out_vars len)
  if(len GREATER 1)
    list(GET out_vars 0 target_src)
    list(GET out_vars 1 target_hdr)
    set(${target_src} ${full_sources} PARENT_SCOPE)
    set(${target_hdr} ${full_headers} PARENT_SCOPE)
  endif()
  if(len GREATER 2)
    list(GET out_vars 2 target_paths)
    if("${family}" STREQUAL "thrift")
      set(${target_paths} ${output_dir} ${output_dir}/include PARENT_SCOPE)
    else()
      set(${target_paths} ${output_dir}/include PARENT_SCOPE)
    endif()
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

  # Match "enum"s, "struct"s and "service"s defined in the file
  string(REGEX MATCHALL "(enum|struct|service)[ \t\n]+([^ \t\n]+)[ \t\n]*{[^}]+}([ \t\n]*\\([^\\)]+\\))?" objects ${file_content})

  # Find object name and append generated files
  foreach(object ${objects})
    string(REGEX MATCH "^(enum|struct|service)[ \t\n]+([^ \t\n{]+)" unused ${object})
    set(objectname ${CMAKE_MATCH_2})
    if(NOT "${object}" MATCHES "{[^}]+}[ \t\n]*(\\([^\\)]*yarp.name[^\\)]+\\))")
      # No files are generated for YARP types.
      list(APPEND gen_srcs ${objectname}.cpp)
      list(APPEND gen_hdrs ${objectname}.h)
    endif()
  endforeach()

  # Remove "enum"s, "struct"s and "service"s
  string(REGEX REPLACE "(enum|struct|service)[ \t\n]+([^ \t\n]+)[ \t\n]*{[^}]+}([ \t\n]*\\([^\\)]+\\))?" "" file_content ${file_content})

  # Find if at least one "const" or "typedef" is defined
  if("${file_content}" MATCHES "(const|typedef)[ \t]+([^ \t\n]+)[ \t]*([^ \t\n]+)")
    list(APPEND gen_hdrs ${basename}_common.h)
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


function(YARP_ADD_IDL var first_file)

  # Ensure that the output variable is empty
  unset(${var})
  unset(include_dirs)

  if("${first_file}" STREQUAL "NO_RECURSE")
    set(_files ${ARGN})
    set(_no_recurse --no-recurse)
  else()
    set(_files "${first_file}" ${ARGN})
    set(_no_recurse )
  endif()

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
    set(tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/yarpidl_${family}")
    string(REGEX REPLACE "/(/|$)" "\\1" tmp_dir "${tmp_dir}")
    make_directory(${tmp_dir})

    # Set output directories and remove extra "/"
    set(srcs_out_dir "${CMAKE_CURRENT_BINARY_DIR}/src")
    set(hdrs_out_dir "${CMAKE_CURRENT_BINARY_DIR}/include")
    string(REGEX REPLACE "/(/|$)" "\\1" srcs_out_dir "${srcs_out_dir}")
    string(REGEX REPLACE "/(/|$)" "\\1" hdrs_out_dir "${hdrs_out_dir}")

    # Prepare main command
    if("${family}" STREQUAL "thrift")
      set(cmd ${YARPIDL_thrift_COMMAND} --gen yarp:include_prefix --I "${CMAKE_CURRENT_SOURCE_DIR}" --out "${tmp_dir}" "${file}")
    else()
      if(YARPIDL_rosmsg_VERBOSE)
        set(_verbose --verbose)
      endif()
      set(cmd ${YARPIDL_rosmsg_COMMAND} --no-ros true --no-cache --no-index ${_verbose} ${_no_recurse} --out "${CMAKE_CURRENT_BINARY_DIR}/include" "${file}")
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
