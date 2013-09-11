# Copyright: (C) 2012 IITRBCS
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

macro(YARP_IDL thrift)
    string(REGEX REPLACE "[^a-zA-Z0-9]" "_" thrift_name ${thrift})
    set(dir ${CMAKE_CURRENT_BINARY_DIR}/${thrift_name})

    set(INCLUDES)
    set(DEST_FILES)
    set(SRC_FILES)
    set(MISSING FALSE)
    set(RENAMING FALSE)

    set(CORE_ARGN)
    set(INCLUDING FALSE)
    foreach(arg ${ARGN})
        if(INCLUDING)
            list(APPEND INCLUDES ${arg})
        else(INCLUDING)
            if(arg STREQUAL "INCLUDE")
                set(INCLUDING TRUE)
            else(arg STREQUAL "INCLUDE")
                list(APPEND CORE_ARGN ${arg})
            endif(arg STREQUAL "INCLUDE")
        endif(INCLUDING)
    endforeach(arg)

    foreach(arg ${CORE_ARGN})
        if(NOT arg STREQUAL "AS")
            get_filename_component(abs_name ${arg} ABSOLUTE)
            if(NOT RENAMING)
                list(APPEND DEST_FILES ${abs_name})
                if(NOT EXISTS ${abs_name})
                    set(MISSING TRUE)
                endif(NOT EXISTS ${abs_name})
            endif(NOT RENAMING)
            get_filename_component(ext ${arg} EXT)
            if(ext STREQUAL ".h")
                list(APPEND INCLUDES ${arg})
            endif(ext STREQUAL ".h")
            set(RENAMING FALSE)
        else(NOT arg STREQUAL "AS")
            set(RENAMING TRUE)
            list(LENGTH INCLUDES len)
            math(EXPR len "${len} - 1")
            list(REMOVE_AT INCLUDES ${len})
        endif(NOT arg STREQUAL "AS")
    endforeach(arg)

    option(ALLOW_IDL_GENERATION "Allow YARP to (re)build IDL files as needed" ${MISSING})
    if(ALLOW_IDL_GENERATION OR MISSING)
        find_package(YARP REQUIRED)
        find_program(YARPIDL_LOCATION yarpidl_thrift HINTS ${YARP_IDL_BINARY_HINT})
        if(NOT YARPIDL_LOCATION)
            message(STATUS "Hints for yarpidl_thrift location: ${YARP_IDL_BINARY_HINT}")
            message(FATAL_ERROR "Cannot find yarpidl_thrift program")
        endif(NOT YARPIDL_LOCATION)
        set(RENAMING FALSE)
        foreach(arg ${CORE_ARGN})
            if(NOT arg STREQUAL "AS")
                get_filename_component(abs_name ${arg} ABSOLUTE)
                get_filename_component(name ${arg} NAME)
                if(NOT RENAMING)
                    list(APPEND SRC_FILES ${name})
                endif(NOT RENAMING)
                set(RENAMING FALSE)
            else(NOT arg STREQUAL "AS")
                set(RENAMING TRUE)
            endif(NOT arg STREQUAL "AS")
        endforeach(arg)
        set(SRC_DIR ${dir})
        get_filename_component(thrift_base ${thrift} NAME_WE)
        set(SRC_INDEX ${thrift_base}_index.txt)
        set(SRC_NAME ${thrift_base})
        configure_file(${YARP_MODULE_DIR}/template/YarpTweakIDL.cmake.in ${dir}/tweak.cmake @ONLY)

        get_filename_component(path_trift ${thrift} ABSOLUTE)
        if(NOT path_thrift)
            set(path_thrift ${CMAKE_CURRENT_SOURCE_DIR})
        endif(NOT path_thrift)
        get_filename_component(name_thrift ${thrift} NAME)
        set(abs_thrift "${path_thrift}/${name_thrift}")
        set(ABS_ARGN)
        foreach(arg ${SRC_FILES})
            set(abs_arg "${dir}/${arg}")
            list(APPEND ABS_ARGN ${abs_arg})
        endforeach(arg)

        add_custom_command(OUTPUT ${DEST_FILES}
                           COMMAND ${CMAKE_COMMAND} -E make_directory ${dir}
                           COMMAND ${YARPIDL_LOCATION} -out ${dir} --gen yarp:cmake_supplies_headers ${abs_thrift}
                           COMMAND ${CMAKE_COMMAND} -P ${dir}/tweak.cmake
                           DEPENDS ${thrift} ${dir}/tweak.cmake)
    endif(ALLOW_IDL_GENERATION OR MISSING)
endmacro(YARP_IDL)


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
	  message(FATAL_ERROR "yarpidl_${family} failed, aborting.")
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
