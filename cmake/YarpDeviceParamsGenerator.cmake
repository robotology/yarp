# SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

function(generateDeviceParamsParser CLASSNAME)

option(ALLOW_DEVICE_PARAM_PARSER_GENERATION "Allow YARP to (re)build device param parsers" OFF)

set(EXECUTABLE_PATH "yarpDeviceParamsParserGen")

  if(ALLOW_DEVICE_PARAM_PARSER_GENERATION)
    message (STATUS "Device param parsers will be regenerated")

    execute_process(
    COMMAND ${EXECUTABLE_PATH} --class_name ${CLASSNAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE result)

    if(result EQUAL 0)
        message(STATUS "Device param parsers generation successful")
    else()
        message(FATAL_ERROR "Device param parsers generation failed")
    endif()

  else()

  endif()
endfunction()
