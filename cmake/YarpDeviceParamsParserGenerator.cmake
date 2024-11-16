# SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# Here is an example of how the YarpDeviceParamParserGenerator should be invoked:
# YarpDeviceParamParserGenerator --class_name className --module_name deviceName --input_filename_md  filename.md    [--input_extra_comments comments.md] [--generate_md] [--generate_ini] [--generate_yarpdev] [--generate_yarprobotinterface] [--generate_all] [--output_dir output_path]
# YarpDeviceParamParserGenerator --class_name className --module_name deviceName --input_filename_ini filename.ini   [--input_extra_comments comments.md] [--generate_md] [--generate_ini] [--generate_yarpdev] [--generate_yarprobotinterface] [--generate_all] [--output_dir output_path]

function(generateDeviceParamsParser_commandline COMMANDLINE)
    option(ALLOW_DEVICE_PARAM_PARSER_GENERATION "Allow YARP to (re)build device param parsers" OFF)

    set(PP_EXECUTABLE "yarpDeviceParamParserGenerator")
    if(ALLOW_DEVICE_PARAM_PARSER_GENERATION)
        message (STATUS "ALLOW_DEVICE_PARAM_PARSER_GENERATION on. Param parsers of device ${CLASSNAME} will be regenerated")

        #list(APPEND PP_PATH_HINT "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/${_config}")
        #find_program (YARP_DEVICE_PARAMS_PARSER_GEN_FOUND ${PP_EXECUTABLE} HINTS ${PP_PATH_HINT} NO_DEFAULT_PATH)

        find_program (YARP_DEVICE_PARAMS_PARSER_GEN_FOUND ${PP_EXECUTABLE})

        message (DEBUG "Executable path: ${YARP_DEVICE_PARAMS_PARSER_GEN_FOUND}" )

        if (YARP_DEVICE_PARAMS_PARSER_GEN_FOUND)
            string(REGEX MATCHALL "[^ ]+" COMMAND_LIST "${COMMANDLINE}")
            execute_process(COMMAND ${PP_EXECUTABLE} ${COMMAND_LIST}
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


function(generateDeviceParamsParser_fromMdFile CLASSNAME DEVICENAME FILEMD EXTRACOMMENTS)
    if (EXTRACOMMENTS)
        set (COMMAND "--class_name ${CLASSNAME} --module_name ${DEVICENAME} --input_filename_md ${FILEMD} --input_extra_comments ${EXTRACOMMENTS}")
    else()
        set (COMMAND "--class_name ${CLASSNAME} --module_name ${DEVICENAME} --input_filename_md ${FILEMD}")
    endif()
    generateDeviceParamsParser_commandline (${COMMAND})
endfunction()

function(generateDeviceParamsParser_fromIniFile CLASSNAME DEVICENAME FILEINI EXTRACOMMENTS)
    if (EXTRACOMMENTS)
        set (COMMAND "--class_name ${CLASSNAME} --module_name ${DEVICENAME} --input_filename_ini ${FILEINI} --input_extra_comments ${EXTRACOMMENTS}")
    else()
        set (COMMAND "--class_name ${CLASSNAME} --module_name ${DEVICENAME} --input_filename_ini ${FILEINI}")
    endif()
    generateDeviceParamsParser_commandline (${COMMAND})
endfunction()

function(generateDeviceParamsParser CLASSNAME DEVICENAME)
    set (INPUTFILENAME_INI "${CMAKE_CURRENT_SOURCE_DIR}/${CLASSNAME}_params.ini")
    set (INPUTFILENAME_MD "${CMAKE_CURRENT_SOURCE_DIR}/${CLASSNAME}_params.md")
    set (INPUTFILENAME_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/${CLASSNAME}_params_extracomments.md")

    if (EXISTS ${INPUTFILENAME_INI})
        set (COMMAND "--class_name ${CLASSNAME} --module_name ${DEVICENAME} --input_filename_ini ${INPUTFILENAME_INI}")
    elseif (EXISTS ${INPUTFILENAME_MD})
        set (COMMAND "--class_name ${CLASSNAME} --module_name ${DEVICENAME} --input_filename_md  ${INPUTFILENAME_MD}")
    else()
        message(FATAL_ERROR "Cannot find input file ${INPUTFILENAME_INI} or ${INPUTFILENAME_MD}")
    endif()

    if (EXISTS $(INPUTFILENAME_EXTRA))
        set (COMMAND "${COMMAND} --input_extra_comments $(INPUTFILENAME_EXTRA)")
    endif()

    generateDeviceParamsParser_commandline (${COMMAND})
endfunction()
