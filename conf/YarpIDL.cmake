# Copyright (C)  2012 Robotics Brain and Cognitive Sciences, Istituto Italiano di Tecnologia
# Copyright (C)  2014 iCub Facility, Istituto Italiano di Tecnologia
# Authors: Elena Ceseracciu, Paul Fitzpatrick, Daniele E. Domenichelli
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# yarp_idl_to_dir
# ---------------
#
# Take an IDL file and generate code for it in the specified directory,
# optionally storing the list of source/header files in the supplied
# variables. Call as:
#
#   yarp_idl_to_dir(foo.thrift foo)
#   yarp_idl_to_dir(foo.thrift foo SOURCES HEADERS)
#   yarp_idl_to_dir(foo.thrift foo SOURCES HEADERS INCLUDE_PATHS)
#
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
if(COMMAND yarp_add_idl)
  return()
endif()

function(YARP_IDL_TO_DIR yarpidl_file_base output_dir)
    # Store optional output variable(s).
    set(out_vars ${ARGN})

    # Make sure output_dir variable is visible when expanding templates.
    set(output_dir ${output_dir})

    # Extract a name and extension.
    if (IS_ABSOLUTE ${yarpidl_file_base})
      file(RELATIVE_PATH yarpidl_file ${CMAKE_CURRENT_SOURCE_DIR} ${yarpidl_file_base})
    else()
      set(yarpidl_file ${yarpidl_file_base})
    endif()
    get_filename_component(include_prefix ${yarpidl_file} PATH)
    get_filename_component(yarpidlName ${yarpidl_file} NAME_WE)
    get_filename_component(yarpidlExt ${yarpidl_file} EXT)
    string(TOLOWER ${yarpidlExt} yarpidlExt)
    string(TOLOWER ${yarpidlName} yarpidlNameLower)
    set(dir_add "")

    # Figure out format we are working with.
    set(family none)
    if (yarpidlExt STREQUAL ".thrift")
        set(family thrift)
        set(dir_add "/${yarpidlNameLower}")
    endif ()
    if (yarpidlExt STREQUAL ".msg")
        set(family rosmsg)
    endif ()
    if (yarpidlExt STREQUAL ".srv")
        set(family rosmsg)
    endif ()
    if (family STREQUAL "none")
        message(FATAL_ERROR "yarp_idl_to_dir does not know what to do with ${yarpidl_file}, unrecognized extension ${yarpidlExt}")
    endif ()

    if("${family}" STREQUAL "thrift")
        set(yarpidl_target_name "${yarpidl_file}")
    else()
        get_filename_component(rospkg_name "${include_prefix}" NAME)
        get_filename_component(include_prefix "${include_prefix}" PATH)
        set(yarpidl_target_name "${rospkg_name}_${yarpidlName}${yarpidlExt}")
    endif()
    string(REGEX REPLACE "[^a-zA-Z0-9]" "_" yarpidl_target_name ${yarpidl_target_name})

    string(LENGTH "${include_prefix}" include_prefix_len)
    if (include_prefix_len GREATER 0)
      set(include_prefix "/${include_prefix}")
    endif ()

    # Set intermediate output directory.
    set(dir ${CMAKE_CURRENT_BINARY_DIR}/_yarp_idl_${include_prefix}${dir_add})
    set(settings_file ${output_dir}/${yarpidl_target_name}.cmake)

    # Check if generation has never happened.
    set(files_missing TRUE)
    if (EXISTS ${settings_file})
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
        find_program(YARPIDL_${family}_LOCATION yarpidl_${family} HINTS ${YARP_IDL_BINARY_HINT})
        # Make sure intermediate output directory exists.
        make_directory(${dir})
        # Generate a script controlling final layout of files.
        configure_file(${YARP_MODULE_DIR}/template/placeGeneratedYarpIdlFiles.cmake.in ${dir}/place${yarpidlName}.cmake @ONLY)
        # Go ahead and generate files.
        execute_process(COMMAND ${YARPIDL_${family}_LOCATION} --out ${dir} --gen yarp:include_prefix --I ${CMAKE_CURRENT_SOURCE_DIR} ${yarpidl_file}
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE res
                        OUTPUT_QUIET
                        ERROR_QUIET)
        # Failure is bad news, let user know.
        if (NOT "${res}" STREQUAL "0")
            message(FATAL_ERROR "yarpidl_${family} (${YARPIDL_${family}_LOCATION}) failed, aborting.")
        endif()
        # Place the files in their final location.
        execute_process(COMMAND ${CMAKE_COMMAND} -P ${dir}/place${yarpidlName}.cmake)
        set(files_missing FALSE)
    endif()

    # Prepare list of generated files.
    if (NOT files_missing)
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
        add_custom_command(OUTPUT ${output_dir}/${yarpidl_target_name}.cmake ${DEST_FILES}
                           COMMAND ${YARPIDL_${family}_LOCATION} --out ${dir} --gen yarp:include_prefix --I ${CMAKE_CURRENT_SOURCE_DIR} ${yarpidl_file}
                           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                           COMMAND ${CMAKE_COMMAND} -P ${dir}/place${yarpidlName}.cmake
                           DEPENDS ${yarpidl_file} ${YARPIDL_LOCATION})
        add_custom_target(${yarpidl_target_name} DEPENDS ${output_dir}/${yarpidl_target_name}.cmake)
    else ()
        if (files_missing)
            message(FATAL_ERROR "Generated IDL files for ${yarpidl_file} not found and cannot make them because ALLOW_IDL_GENERATION=${ALLOW_IDL_GENERATION} (maybe this should be turned on?)")
        else ()
            message(STATUS "Not processing ${family} file ${yarpidl_file}, ALLOW_IDL_GENERATION=${ALLOW_IDL_GENERATION}")
        endif ()
    endif(ALLOW_IDL_GENERATION)

    list(LENGTH out_vars len)
    if (len GREATER 1)
        list(GET out_vars 0 target_src)
        list(GET out_vars 1 target_hdr)
        set(${target_src} ${full_sources} PARENT_SCOPE)
        set(${target_hdr} ${full_headers} PARENT_SCOPE)
    endif()
    if (len GREATER 2)
        list(GET out_vars 2 target_paths)
        set(${target_paths} ${output_dir} ${output_dir}/include PARENT_SCOPE)
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
  string (REGEX REPLACE "(enum|struct|service)[ \t\n]+([^ \t\n]+)[ \t\n]*{[^}]+}([ \t\n]*\\([^\\)]+\\))?" "" file_content ${file_content})

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

  if(NOT "${pkg}" STREQUAL "")
    set(gen_hdrs "${pkg}/${basename}.h"
                 "${pkg}_${basename}.h")
  else()
    set(gen_hdrs "${basename}.h")
  endif()
  if("${ext}" STREQUAL ".srv")
    list(APPEND gen_hdrs ${basename}Reply.h)
    if(NOT "${path}" STREQUAL "")
      list(APPEND gen_hdrs ${clean_path}_${basename}Reply.h)
    endif()
  endif()

  # Read rosmsg file
  file(READ ${file} file_content)

  # Check if std_msgs/Header.h or TickTime.h will be created
  if("${file_content}" MATCHES "(^|\n)Header[ \t]")
    list(APPEND gen_hdrs std_msgs/Header.h std_msgs_Header.h TickTime.h)
  elseif("${file_content}" MATCHES "(^|\n)time[ \t]")
    list(APPEND gen_hdrs TickTime.h)
  endif()

  set(${gen_srcs_var} ${gen_srcs} PARENT_SCOPE)
  set(${gen_hdrs_var} ${gen_hdrs} PARENT_SCOPE)
endfunction()


function(YARP_ADD_IDL var first_file)

  # Ensure that the output variable is empty
  unset(${var})
  unset(include_dirs)

  foreach(file "${first_file}" ${ARGN})

    # Ensure that the filename is relative to the current source directory
    if (IS_ABSOLUTE "${file}")
      file(RELATIVE_PATH file "${CMAKE_CURRENT_SOURCE_DIR}" "${file}")
    endif()

    # Extract a name and extension.
    get_filename_component(path ${file} PATH)
    get_filename_component(basename ${file} NAME_WE)
    get_filename_component(ext ${file} EXT)
    string(TOLOWER ${ext} ext)

    # Figure out format we are working with and determine which files
    # will be generated
    if("${ext}" STREQUAL ".thrift")
      set(family thrift)
      _yarp_idl_thrift_to_file_list("${file}" "${path}" "${basename}" ${ext} gen_srcs gen_hdrs)
    elseif("${ext}" MATCHES "^\\.(msg|srv)$")
      set(family rosmsg)
      get_filename_component(pkg "${path}" NAME)
      get_filename_component(path "${path}" PATH)
      _yarp_idl_rosmsg_to_file_list("${file}" "${path}" "${pkg}" "${basename}" ${ext} gen_srcs gen_hdrs)
    else()
        message(FATAL_ERROR "Unknown extension ${ext}. Supported extensiona are .thrift, .msg, and .srv")
    endif()


    # Choose target depending on family and on whether we are building
    # or using YARP
    # FIXME CMake 2.8.12 Use ALIAS and always YARP::yarpidl_${family}
    if(TARGET YARP::yarpidl_${family})
        # Outside YARP
        set(YARPIDL_${family}_COMMAND YARP::yarpidl_${family})
    else()
        # Building YARP
        set(YARPIDL_${family}_COMMAND yarpidl_${family})
    endif()

    # Set intermediate output directory, remove extra '/' and ensure that
    # the directory exists.
    set(tmp_dir "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/yarpidl_${family}/${path}")
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
      set(cmd ${YARPIDL_rosmsg_COMMAND} --no-ros true --out "${CMAKE_CURRENT_BINARY_DIR}/include" "${file}")
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
      set(out "${hdrs_out_dir}/${path}/${gen_file}")
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
      add_custom_command(OUTPUT ${output}
                         DEPENDS ${file}
                                 ${extra_deps}
                         COMMAND ${cmd}
                         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                         COMMENT "Generating code from ${file}")

      # Append output to return variable
      list(APPEND ${var} ${output})

      # Force CMake to run again if the file is modified
      configure_file("${file}" "${tmp_dir}/${basename}${ext}" COPYONLY)
    endif()

  endforeach()

  set(${var} ${${var}} PARENT_SCOPE)
  include_directories("${CMAKE_CURRENT_BINARY_DIR}/include/")

endfunction()
