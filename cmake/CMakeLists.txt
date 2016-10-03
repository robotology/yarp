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
  configure_file(${YARP_MODULE_DIR}/template/yarp_conf_${conf}.h.in
                  ${outconf})
  message(STATUS "generated ${outconf}")
  install(FILES ${outconf}
          COMPONENT development
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/conf)
endforeach(conf)

# YARP CMake Modules
set(YARP_CMAKE_FILES YarpDeprecatedWarning.cmake
                     YarpPlugin.cmake
                     YarpIDL.cmake
                     YarpInstallationHelpers.cmake)
install(FILES ${YARP_CMAKE_FILES}
        COMPONENT scripts
        DESTINATION share/yarp/cmake)

# Templates used by modules
set(YARP_TEMPLATE_FILES template/yarp_plugin_carrier.cpp.in
                        template/yarp_plugin_device.cpp.in
                        template/yarp_plugin_portmonitor.cpp.in
                        template/yarp_plugin_library.cpp.in
                        template/yarp_plugin_yarpdev_main.cpp.in
                        template/YarpTweakIDL.cmake.in
                        template/placeGeneratedYarpIdlFiles.cmake.in)

install(FILES ${YARP_TEMPLATE_FILES}
        COMPONENT scripts
        DESTINATION share/yarp/cmake/template)

if(NOT YARP_NO_DEPRECATED) # since YARP 2.3.68
  set(YARP_PRIVATE_CMAKE_FILES YarpDeprecatedOption.cmake
                               YarpCMakeWorkarounds.cmake
                               YarpRenamedOption.cmake
                               YarpBackupVariable.cmake)
  install(FILES ${YARP_PRIVATE_CMAKE_FILES}
          COMPONENT scripts
          DESTINATION share/yarp/cmake/deprecated)

  if(NOT YCM_FOUND)
    set(YCM_CMAKE_find_modules_FILES ycm-0.2/find-modules/FindACE.cmake
                                     ycm-0.2/find-modules/FindAtlas.cmake
                                     ycm-0.2/find-modules/FindFFMPEG.cmake
                                     ycm-0.2/find-modules/FindFreenect.cmake
                                     ycm-0.2/find-modules/FindFTDI.cmake
                                     ycm-0.2/find-modules/FindGLFW3.cmake
                                     ycm-0.2/find-modules/FindGSL.cmake
                                     ycm-0.2/find-modules/FindLibedit.cmake
                                     ycm-0.2/find-modules/FindLibOVR.cmake
                                     ycm-0.2/find-modules/FindLibdc1394.cmake
                                     ycm-0.2/find-modules/FindLibusb1.cmake
                                     ycm-0.2/find-modules/FindNVIDIACg.cmake
                                     ycm-0.2/find-modules/FindOpenCV.cmake
                                     ycm-0.2/find-modules/FindOpenNI.cmake
                                     ycm-0.2/find-modules/FindPortAudio.cmake
                                     ycm-0.2/find-modules/FindSQLite.cmake
                                     ycm-0.2/find-modules/FindStage.cmake
                                     ycm-0.2/find-modules/FindTinyXML.cmake)
    set(YCM_CMAKE_modules_FILES ycm-0.2/modules/AddInstallRPATHSupport.cmake
                                ycm-0.2/modules/ExtractVersion.cmake
                                ycm-0.2/modules/GetAllCMakeProperties.cmake
                                ycm-0.2/modules/ReplaceImportedTargets.cmake
                                ycm-0.2/modules/StandardFindModule.cmake)
    set(YCM_CMAKE_3rdparty_FILES ycm-0.2/3rdparty/FindEigen3.cmake
                                 ycm-0.2/3rdparty/FindOctave.cmake
                                 ycm-0.2/3rdparty/FindQCustomPlot.cmake)
    set(YCM_CMAKE_cmake_3_1_FILES ycm-0.2/cmake-3.1/Modules/CMakeFindDependencyMacro.cmake # 3.0
                                  ycm-0.2/cmake-3.1/Modules/CMakePackageConfigHelpers.cmake
                                  ycm-0.2/cmake-3.1/Modules/CMakeParseArguments.cmake
                                  ycm-0.2/cmake-3.1/Modules/FeatureSummary.cmake
                                  ycm-0.2/cmake-3.1/Modules/FindLua.cmake # 3.0
                                  ycm-0.2/cmake-3.1/Modules/FindPackageHandleStandardArgs.cmake
                                  ycm-0.2/cmake-3.1/Modules/FindPackageMessage.cmake
                                  ycm-0.2/cmake-3.1/Modules/GNUInstallDirs.cmake)
    set(YCM_CMAKE_cmake_next_FILES ycm-0.2/cmake-proposed/Modules/FindGLEW.cmake)

    install(FILES ${YCM_CMAKE_find_modules_FILES}
            COMPONENT scripts
            DESTINATION share/yarp/cmake/ycm-0.2/find-modules)
    install(FILES ${YCM_CMAKE_modules_FILES}
            COMPONENT scripts
            DESTINATION share/yarp/cmake/ycm-0.2/modules)
    install(FILES ${YCM_CMAKE_3rdparty_FILES}
            COMPONENT scripts
            DESTINATION share/yarp/cmake/ycm-0.2/3rdparty)
    install(FILES ${YCM_CMAKE_cmake_next_FILES}
            COMPONENT scripts
            DESTINATION share/yarp/cmake/ycm-0.2/cmake-proposed/Modules)
    if(${CMAKE_VERSION} VERSION_LESS 3.1)
      # Files from CMake 3.1, available only if cmake is older
      install(FILES ${YCM_CMAKE_cmake_3_1_FILES}
              COMPONENT scripts
              DESTINATION share/yarp/cmake/ycm-0.2/cmake-3.1/Modules)
    endif()
  endif()
endif()

# Remove renamed files from installation (since YARP 2.3.68)
foreach(_f FindACE.cmake
           FindAtlas.cmake
           FindFFMPEG.cmake
           FindFreenect.cmake
           FindFTDI.cmake
           FindGLEW.cmake
           FindGLFW3.cmake
           FindGSL.cmake
           FindLibedit.cmake
           FindLibOVR.cmake
           FindLibdc1394.cmake
           FindLibusb1.cmake
           FindNVIDIACg.cmake
           FindOctave.cmake
           FindOpenCV.cmake
           FindOpenNI.cmake
           FindPortAudio.cmake
           FindQCustomPlot.cmake
           FindReadLine.cmake
           FindSQLite.cmake
           FindStage.cmake
           FindTinyXML.cmake
           AddInstallRPATHSupport.cmake
           ExtractVersion.cmake
           GetAllCMakeProperties.cmake
           ReplaceImportedTargets.cmake
           StandardFindModule.cmake
           YarpDeprecatedOption.cmake
           YarpCMakeWorkarounds.cmake
           YarpRenamedOption.cmake
           YarpBackupVariable.cmake
           cmake-3.1/Modules/CMakeFindDependencyMacro.cmake
           cmake-3.1/Modules/CMakePackageConfigHelpers.cmake
           cmake-3.1/Modules/CMakeParseArguments.cmake
           cmake-3.1/Modules/FeatureSummary.cmake
           cmake-3.1/Modules/FindLua.cmake
           cmake-3.1/Modules/FindPackageHandleStandardArgs.cmake
           cmake-3.1/Modules/FindPackageMessage.cmake
           cmake-3.1/Modules/GNUInstallDirs.cmake
           ycm-0.2/cmake-next/Modules/FindGLEW.cmake)
  if(EXISTS "${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_f}")
    install(CODE
 "if(EXISTS \"${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_f}\")
    message(STATUS \"Deleted: \\\"${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_f}\\\"\")
    file(REMOVE \"${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_f}\")
  endif()"
           COMPONENT scripts)
  endif()
endforeach()