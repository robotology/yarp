# Copyright: (C) 2012 IITRBCS
# Authors: Elena Ceseracciu, Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Take an IDL file and generate code for it in the specified directory.
macro(YARP_IDL_TO_DIR yarpidl_file output_dir)
    # Make sure output_dir variable is visible when expanding templates.
    set(output_dir ${output_dir})

    # Extract a name and extension.
    get_filename_component(include_prefix ${yarpidl_file} PATH)
    get_filename_component(yarpidlName ${yarpidl_file} NAME_WE)
    get_filename_component(yarpidlExt ${yarpidl_file} EXT)
    string(TOLOWER ${yarpidlExt} yarpidlExt)
    string(TOLOWER ${yarpidlName} yarpidlNameLower)

    # Figure out format we are working with.
    set(family none)
    if (yarpidlExt STREQUAL ".thrift")
      set(family thrift)
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

    # Set intermediate output directory.
    set(dir ${CMAKE_CURRENT_BINARY_DIR}/_yarp_idl_/${include_prefix}/${yarpidlNameLower})
    string(REGEX REPLACE "[^a-zA-Z0-9]" "_" yarpidl_target_name ${yarpidl_file})
    set(settings_file ${output_dir}/${yarpidl_target_name}.cmake)

    # Check if generation has never happened.
    set(files_missing TRUE)
    if (EXISTS ${settings_file})
      set(files_missing FALSE)
    endif()

    # Flag to control whether IDL generation is allowed.
    option(ALLOW_IDL_GENERATION "Allow YARP to (re)build IDL files as needed" ${files_missing})
    mark_as_advanced(ALLOW_IDL_GENERATION)

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
			RESULT_VARIABLE res)
	# Failure is bad news, let user know.
	if (NOT "${res}" STREQUAL "0")
	  message(FATAL_ERROR "yarpidl_${family} (${YARPIDL_${family}_LOCATION}) failed, aborting.")
	endif()
	# Place the files in their final location.
        execute_process(COMMAND ${CMAKE_COMMAND} -P ${dir}/place${yarpidlName}.cmake)
	# Prepare list of generated files.
        include(${settings_file})
        set(DEST_FILES)
        foreach(generatedFile ${headers})
            list(APPEND DEST_FILES ${output_dir}/${generatedFile})
        endforeach(generatedFile)
        foreach(generatedFile ${sources})
            list(APPEND DEST_FILES ${output_dir}/${generatedFile})
        endforeach(generatedFile)

	# Add a command/target to regenerate the files if the IDL file changes.
        add_custom_command(OUTPUT ${output_dir}/${yarpidl_target_name}.cmake ${DEST_FILES}
                           COMMAND ${YARPIDL_${family}_LOCATION} --out ${dir} --gen yarp:include_prefix --I ${CMAKE_CURRENT_SOURCE_DIR} ${yarpidl_file}
                           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                           COMMAND ${CMAKE_COMMAND} -P ${dir}/place${yarpidlName}.cmake
                           DEPENDS ${yarpidl_file} ${YARPIDL_LOCATION})
        add_custom_target(${yarpidl_target_name} DEPENDS ${output_dir}/${yarpidl_target_name}.cmake)
    else ()
        message(STATUS "Not processing ${family} file ${yarpidl_file}, ALLOW_IDL_GENERATION=${ALLOW_IDL_GENERATION}")
    endif(ALLOW_IDL_GENERATION)
endmacro(YARP_IDL_TO_DIR)
