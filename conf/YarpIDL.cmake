# Copyright: (C) 2012 IITRBCS
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

macro(YARP_IDL_TO_DIR yarpidl_file output_dir)
    set(output_dir ${output_dir})

    get_filename_component(include_prefix  ${yarpidl_file} PATH)
    get_filename_component(yarpidlName ${yarpidl_file} NAME_WE)
    get_filename_component(yarpidlExt ${yarpidl_file} EXT)
    string(TOLOWER ${yarpidlExt} yarpidlExt)
    string(TOLOWER ${yarpidlName} yarpidlNameLower)

    # Figure out what kind of IDL we are looking at
    set(family thrift)
    if (yarpidlExt STREQUAL ".msg")
      set(family rosmsg)
    endif ()
    if (yarpidlExt STREQUAL ".srv")
      set(family rosmsg)
    endif ()

    # set output
    set(dir ${CMAKE_CURRENT_BINARY_DIR}/${include_prefix}/${yarpidlNameLower})
    string(REGEX REPLACE "[^a-zA-Z0-9]" "_" yarpidl_target_name ${yarpidl_file})
    option(ALLOW_IDL_GENERATION "Allow YARP to (re)build IDL files as needed" FALSE)
    mark_as_advanced(ALLOW_IDL_GENERATION)
    if(ALLOW_IDL_GENERATION)
        message(STATUS "Generating source files for ${family} file ${yarpidl_file}. Output directory: ${output_dir}.")
        # generate during cmake configuration, so we have all the names of generated files
        find_program(YARPIDL_${family}_LOCATION yarpidl_${family} HINTS ${YARP_IDL_BINARY_HINT})
        make_directory(${dir})
        configure_file(${YARP_MODULE_DIR}/template/placeGeneratedYarpIdlFiles.cmake.in ${dir}/place${yarpidlName}.cmake @ONLY)
        execute_process(COMMAND ${YARPIDL_${family}_LOCATION} --out ${dir} --gen yarp:include_prefix --I ${CMAKE_CURRENT_SOURCE_DIR} ${yarpidl_file}
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			RESULT_VARIABLE res)
	if (NOT "${res}" STREQUAL "0")
	  message(FATAL_ERROR "yarpidl_${family} (${YARPIDL_${family}_LOCATION}) failed, aborting.")
	endif()
        execute_process(COMMAND ${CMAKE_COMMAND} -P ${dir}/place${yarpidlName}.cmake)

        include(${output_dir}/${yarpidl_target_name}.cmake)
        set(DEST_FILES)
        foreach(generatedFile ${headers})
            list(APPEND DEST_FILES ${output_dir}/${generatedFile})
        endforeach(generatedFile)
        foreach(generatedFile ${sources})
            list(APPEND DEST_FILES ${output_dir}/${generatedFile})
        endforeach(generatedFile)

        add_custom_command(OUTPUT ${output_dir}/${yarpidl_target_name}.cmake ${DEST_FILES}
                           COMMAND ${YARPIDL_${family}_LOCATION} --out ${dir} --gen yarp:include_prefix --I ${CMAKE_CURRENT_SOURCE_DIR} ${yarpidl_file}
                           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                           COMMAND ${CMAKE_COMMAND} -P ${dir}/place${yarpidlName}.cmake
                           DEPENDS ${yarpidl_file} ${YARPIDL_LOCATION})
        add_custom_target(${yarpidl_target_name} DEPENDS ${output_dir}/${yarpidl_target_name}.cmake)

    endif(ALLOW_IDL_GENERATION)
endmacro(YARP_IDL_TO_DIR)
