# SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

function(generateDeviceParamsParser CLASSNAME)

option(ALLOW_DEVICE_PARAM_PARSER_GENERATION "Allow YARP to (re)build device param parsers" OFF)

set(PP_EXECUTABLE "yarpDeviceParamParserGenerator")

  if(ALLOW_DEVICE_PARAM_PARSER_GENERATION)
    message (STATUS "ALLOW_DEVICE_PARAM_PARSER_GENERATION on. Param parsers of device ${CLASSNAME} will be regenerated")

    #list(APPEND PP_PATH_HINT "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/${_config}")
    #find_program (YARP_DEVICE_PARAMS_PARSER_GEN_FOUND ${PP_EXECUTABLE} HINTS ${PP_PATH_HINT} NO_DEFAULT_PATH)

    find_program (YARP_DEVICE_PARAMS_PARSER_GEN_FOUND ${PP_EXECUTABLE})

    message (DEBUG "Executable path: ${YARP_DEVICE_PARAMS_PARSER_GEN_FOUND}" )

    if (YARP_DEVICE_PARAMS_PARSER_GEN_FOUND)
        execute_process(
            COMMAND ${PP_EXECUTABLE} --class_name ${CLASSNAME}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE result)

        if(result EQUAL 0)
            message(STATUS "Param parser of device ${CLASSNAME} successfully generated")
        else()
            message(FATAL_ERROR "Generation of param parser of device ${CLASSNAME} failed")
        endif()
    else()
        message (WARNING "yarpDeviceParamParserGenerator executable not found. Is it compiled?\nDevice param parser of device ${CLASSNAME} was not generated.")
    endif()
  endif()
endfunction()
