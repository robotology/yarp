# Copyright: (C) 2009, 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

set_property(GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/generated_include)

foreach(conf version system options api numeric)
    set(outconf ${CMAKE_BINARY_DIR}/generated_include/yarp/conf/${conf}.h)
    configure_file(${YARP_MODULE_DIR}/template/yarp_config_${conf}.h.in
                   ${outconf} IMMEDIATE)
    message(STATUS "generated ${outconf}")
    install(FILES ${outconf} COMPONENT development DESTINATION include/yarp/conf)
endforeach(conf)

# There are several cmake modules we traditionally make available to
# users of YARP.
set(YARP_CMAKE_FILES YarpPlugin.cmake
                     YarpIDL.cmake
                     YarpDevice.cmake
                     MacroExtractVersion.cmake
                     MacroStandardFindModule.cmake
                     YarpDeprecatedWarning.cmake
                     YarpInstallationHelpers.cmake
                     YarpCMakeWorkarounds.cmake
                     FindACE.cmake
                     FindAtlas.cmake
                     FindFFMPEG.cmake
                     FindGooCanvas.cmake
                     FindGooCanvasMM.cmake
                     FindGSL.cmake
                     FindGtkDatabox.cmake
                     FindGtkDataboxMM.cmake
                     FindOpenCV.cmake
                     FindPortAudio.cmake
                     FindSQLite.cmake
                     FindTinyXML.cmake)

install(FILES ${YARP_CMAKE_FILES}
       COMPONENT scripts
       DESTINATION share/yarp/cmake)


# Templates used by modules
set(YARP_TEMPLATE_FILES template/yarp_plugin_carrier.cpp.in
                        template/yarp_plugin_device.cpp.in
                        template/yarpdev_lib.cpp.in
                        template/yarpdev_lib.h.in
                        template/yarpdev_lib_main.cpp.in
                        template/YarpTweakIDL.cmake.in
                        template/placeGeneratedYarpIdlFiles.cmake.in)

install(FILES ${YARP_TEMPLATE_FILES}
        COMPONENT scripts
        DESTINATION share/yarp/cmake/template)


# Files from CMake 2.8.12, available only if cmake is older
if(CMAKE_VERSION VERSION_LESS 2.8.12)
    set(YARP_CMAKE_2_8_12_FILES cmake-2.8.12/Modules/FindGTK2.cmake
                                cmake-2.8.12/Modules/FindPackageHandleStandardArgs.cmake
                                cmake-2.8.12/Modules/FindPackageMessage.cmake
                                cmake-2.8.12/Modules/CMakeParseArguments.cmake
                                cmake-2.8.12/Modules/SelectLibraryConfigurations.cmake
                                cmake-2.8.12/Modules/FindFreetype.cmake)
    install(FILES ${YARP_CMAKE_2_8_12_FILES}
            COMPONENT scripts
            DESTINATION share/yarp/cmake/cmake-2.8.12/Modules)
endif()


# Deprecated modules installed for compatibility
if(NOT YARP_NO_DEPRECATED)
    set(YARP_DEPRECATED_CMAKE_FILES deprecated/FindGthread.cmake
                                    deprecated/FindGtkMM.cmake
                                    deprecated/FindGtkMMUnix.cmake
                                    deprecated/FindGtkMMWin32.cmake
                                    deprecated/FindGtkPlus.cmake
                                    deprecated/FindGtkWin32.cmake)
    install(FILES ${YARP_DEPRECATED_CMAKE_FILES}
            COMPONENT scripts
            DESTINATION share/yarp/cmake/deprecated)
endif()
