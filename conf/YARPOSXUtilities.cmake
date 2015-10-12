# Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
# Author: Francesco Romano <francesco.romano@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


function(yarp_osx_bundle_update_rpath _target_name)
    if(INSTALL_WITH_RPATH OR ENABLE_FORCE_RPATH)
        get_target_property(APPNAME "${_target_name}" OUTPUT_NAME)
        file(RELATIVE_PATH _rel_path "${CMAKE_INSTALL_FULL_BINDIR}/${APPNAME}.app/Contents/MacOS/" "${CMAKE_INSTALL_FULL_LIBDIR}")
        if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            get_target_property(CURRENT_RPATH "${_target_name}" INSTALL_RPATH)
            list(APPEND CURRENT_RPATH "@loader_path/${_rel_path}")
            set_target_properties("${_target_name}" PROPERTIES INSTALL_RPATH "${CURRENT_RPATH}")
            get_target_property(CURRENT_RPATH "${_target_name}" INSTALL_RPATH)
        endif()
    endif()
endfunction()

function(DUPLICATE_AND_ADD_BUNDLE)
include(CMakeParseArguments)

set(_options)
set(_oneValueArgs TARGET OUTPUT_NAME APP_ICON APP_ICON_PATH)
set(_multiValueArgs )

cmake_parse_arguments(DAD "${_options}"
                           "${_oneValueArgs}"
                           "${_multiValueArgs}"
                           "${ARGN}")

get_target_property(SOURCES ${DAD_TARGET} SOURCES)
if (DEFINED DAD_APP_ICON)
    file(TO_CMAKE_PATH "${DAD_APP_ICON_PATH}/${DAD_APP_ICON}" APP_FULL_PATH)
endif()

add_executable(${DAD_OUTPUT_NAME} ${SOURCES} ${APP_FULL_PATH})
set_target_properties(${DAD_OUTPUT_NAME} PROPERTIES MACOSX_BUNDLE TRUE)

get_target_property(LINKS ${DAD_TARGET} LINK_LIBRARIES)
target_link_libraries(${DAD_OUTPUT_NAME} ${LINKS})

if (DEFINED DAD_APP_ICON)
    set_target_properties(${DAD_OUTPUT_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE ${DAD_APP_ICON})
    # 2 - Copy the resource in the Resources folder in the bundle
    set_source_files_properties(${APP_FULL_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

yarp_osx_bundle_update_rpath(${DAD_OUTPUT_NAME})
endfunction()
