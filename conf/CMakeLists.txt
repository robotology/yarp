# Copyright: (C) 2009, 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

set_property(GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/generated_include)

# compiler.h is generated in YarpSystemCheck.cmake
install(FILES ${CMAKE_BINARY_DIR}/generated_include/yarp/conf/compiler.h
        COMPONENT development
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/conf)

foreach(conf version system options api numeric)
    set(outconf ${CMAKE_BINARY_DIR}/generated_include/yarp/conf/${conf}.h)
    configure_file(${YARP_MODULE_DIR}/template/yarp_config_${conf}.h.in
                   ${outconf})
    message(STATUS "generated ${outconf}")
    install(FILES ${outconf}
            COMPONENT development
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/conf)
endforeach(conf)

# There are several cmake modules we traditionally make available to
# users of YARP.
set(YARP_CMAKE_FILES YarpPlugin.cmake
                     YarpIDL.cmake
                     ExtractVersion.cmake
                     ReplaceImportedTargets.cmake
                     StandardFindModule.cmake
                     YarpDeprecatedWarning.cmake
                     YarpDeprecatedOption.cmake
                     YarpInstallationHelpers.cmake
                     YarpCMakeWorkarounds.cmake
                     YarpRenamedOption.cmake
                     YarpBackupVariable.cmake
                     FindACE.cmake
                     FindAtlas.cmake
                     FindFFMPEG.cmake
                     FindFreenect.cmake
                     FindFTDI.cmake
                     FindGSL.cmake
                     FindLibdc1394.cmake
                     FindLibusb1.cmake
                     FindOpenCV.cmake
                     FindOpenNI.cmake
                     FindPortAudio.cmake
                     FindReadline.cmake
                     FindSQLite.cmake
                     FindStage.cmake
                     FindTinyXML.cmake
                     AddInstallRPATHSupport.cmake)

install(FILES ${YARP_CMAKE_FILES}
       COMPONENT scripts
       DESTINATION share/yarp/cmake)


# Templates used by modules
set(YARP_TEMPLATE_FILES template/yarp_plugin_carrier.cpp.in
                        template/yarp_plugin_device.cpp.in
                        template/yarp_plugin_portmonitor.cpp.in
                        template/yarp_plugin_lib.cpp.in
                        template/yarp_plugin_yarpdev_main.cpp.in
                        template/YarpTweakIDL.cmake.in
                        template/placeGeneratedYarpIdlFiles.cmake.in)

install(FILES ${YARP_TEMPLATE_FILES}
        COMPONENT scripts
        DESTINATION share/yarp/cmake/template)

# Files from CMake 3.1, available only if cmake is older
if(${CMAKE_VERSION} VERSION_LESS 3.1)
    set(YARP_CMAKE_3_1_FILES cmake-3.1/Modules/CMakeFindDependencyMacro.cmake # 3.0
                             cmake-3.1/Modules/CMakePackageConfigHelpers.cmake
                             cmake-3.1/Modules/CMakeParseArguments.cmake
                             cmake-3.1/Modules/FeatureSummary.cmake
                             cmake-3.1/Modules/FindLua.cmake # 3.0
                             cmake-3.1/Modules/FindPackageHandleStandardArgs.cmake
                             cmake-3.1/Modules/FindPackageMessage.cmake
                             cmake-3.1/Modules/GNUInstallDirs.cmake)
    install(FILES ${YARP_CMAKE_3_1_FILES}
            COMPONENT scripts
            DESTINATION share/yarp/cmake/cmake-3.1/Modules)
endif()
