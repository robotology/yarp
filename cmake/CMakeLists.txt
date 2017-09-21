# Copyright: (C) 2009, 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

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
                               YarpRenamedOption.cmake
                               YarpBackupVariable.cmake)
  install(FILES ${YARP_PRIVATE_CMAKE_FILES}
          COMPONENT scripts
          DESTINATION share/yarp/cmake/deprecated)

  if(NOT YCM_FOUND)
    set(YCM_CMAKE_find_modules_FILES ycm-${YCM_REQUIRED_VERSION}/find-modules/FindACE.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/find-modules/FindFFMPEG.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/find-modules/FindFreenect.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/find-modules/FindFTDI.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/find-modules/FindGLFW3.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/find-modules/FindLibedit.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/find-modules/FindLibOVR.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/find-modules/FindLibdc1394.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/find-modules/FindLibusb1.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/find-modules/FindNVIDIACg.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/find-modules/FindOpenCV.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/find-modules/FindOpenNI.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/find-modules/FindOpenNI2.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/find-modules/FindPortAudio.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/find-modules/FindSQLite.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/find-modules/FindStage.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/find-modules/FindTinyXML.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/find-modules/FindZFP.cmake)
    set(YCM_CMAKE_modules_FILES ycm-${YCM_REQUIRED_VERSION}/modules/AddInstallRPATHSupport.cmake
                                ycm-${YCM_REQUIRED_VERSION}/modules/ExtractVersion.cmake
                                ycm-${YCM_REQUIRED_VERSION}/modules/GetAllCMakeProperties.cmake
                                ycm-${YCM_REQUIRED_VERSION}/modules/GitInfo.cmake
                                ycm-${YCM_REQUIRED_VERSION}/modules/ReplaceImportedTargets.cmake
                                ycm-${YCM_REQUIRED_VERSION}/modules/StandardFindModule.cmake)
    set(YCM_CMAKE_3rdparty_FILES ycm-${YCM_REQUIRED_VERSION}/3rdparty/FindEigen3.cmake
                                 ycm-${YCM_REQUIRED_VERSION}/3rdparty/FindOctave.cmake
                                 ycm-${YCM_REQUIRED_VERSION}/3rdparty/FindQCustomPlot.cmake)
    set(YCM_CMAKE_cmake_3_1_FILES ycm-${YCM_REQUIRED_VERSION}/cmake-3.1/Modules/CMakeFindDependencyMacro.cmake # 3.0
                                  ycm-${YCM_REQUIRED_VERSION}/cmake-3.1/Modules/CMakePackageConfigHelpers.cmake
                                  ycm-${YCM_REQUIRED_VERSION}/cmake-3.1/Modules/CMakeParseArguments.cmake
                                  ycm-${YCM_REQUIRED_VERSION}/cmake-3.1/Modules/FeatureSummary.cmake
                                  ycm-${YCM_REQUIRED_VERSION}/cmake-3.1/Modules/FindLua.cmake # 3.0
                                  ycm-${YCM_REQUIRED_VERSION}/cmake-3.1/Modules/FindPackageHandleStandardArgs.cmake
                                  ycm-${YCM_REQUIRED_VERSION}/cmake-3.1/Modules/FindPackageMessage.cmake
                                  ycm-${YCM_REQUIRED_VERSION}/cmake-3.1/Modules/GNUInstallDirs.cmake)
    set(YCM_CMAKE_cmake_next_FILES ycm-${YCM_REQUIRED_VERSION}/cmake-next/Modules/FindSWIG.cmake
                                   ycm-${YCM_REQUIRED_VERSION}/cmake-next/Modules/UseSWIG.cmake)
    set(YCM_CMAKE_cmake_proposed_FILES ycm-${YCM_REQUIRED_VERSION}/cmake-proposed/Modules/FindGLEW.cmake)

    install(FILES ${YCM_CMAKE_find_modules_FILES}
            COMPONENT scripts
            DESTINATION share/yarp/cmake/ycm-${YCM_REQUIRED_VERSION}/find-modules)
    install(FILES ${YCM_CMAKE_modules_FILES}
            COMPONENT scripts
            DESTINATION share/yarp/cmake/ycm-${YCM_REQUIRED_VERSION}/modules)
    install(FILES ${YCM_CMAKE_3rdparty_FILES}
            COMPONENT scripts
            DESTINATION share/yarp/cmake/ycm-${YCM_REQUIRED_VERSION}/3rdparty)
    install(FILES ${YCM_CMAKE_cmake_proposed_FILES}
            COMPONENT scripts
            DESTINATION share/yarp/cmake/ycm-${YCM_REQUIRED_VERSION}/cmake-proposed/Modules)
    if(${CMAKE_VERSION} VERSION_LESS 3.1)
      # Files from CMake 3.1, available only if cmake is newer
      install(FILES ${YCM_CMAKE_cmake_3_1_FILES}
              COMPONENT scripts
              DESTINATION share/yarp/cmake/ycm-${YCM_REQUIRED_VERSION}/cmake-3.1/Modules)
    endif()
    if(${CMAKE_VERSION} VERSION_LESS 3.8)
      # Files from CMake next (3.8), available only if cmake is newer
      install(FILES ${YCM_CMAKE_cmake_next_FILES}
              COMPONENT scripts
              DESTINATION share/yarp/cmake/ycm-${YCM_REQUIRED_VERSION}/cmake-next/Modules)
    endif()
  endif()
endif()

# Remove renamed files from installation
foreach(_f FindACE.cmake                                          # since YARP 2.3.68
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
           deprecated/FindGthread.cmake
           deprecated/FindGtkMM.cmake
           deprecated/FindGtkMMUnix.cmake
           deprecated/FindGtkMMWin32.cmake
           deprecated/FindGtkPlus.cmake
           deprecated/FindGtkWin32.cmake
           deprecated/MacroExtractVersion.cmake
           deprecated/MacroStandardFindModule.cmake
           cmake-3.1/Modules/CMakeFindDependencyMacro.cmake
           cmake-3.1/Modules/CMakePackageConfigHelpers.cmake
           cmake-3.1/Modules/CMakeParseArguments.cmake
           cmake-3.1/Modules/FeatureSummary.cmake
           cmake-3.1/Modules/FindLua.cmake
           cmake-3.1/Modules/FindPackageHandleStandardArgs.cmake
           cmake-3.1/Modules/FindPackageMessage.cmake
           cmake-3.1/Modules/GNUInstallDirs.cmake
           cmake-3.5/Modules/CMakeParseArguments.cmake
           cmake-3.5/Modules/FindFreetype.cmake
           cmake-3.5/Modules/FindGTK2.cmake
           cmake-3.5/Modules/FindPackageHandleStandardArgs.cmake
           cmake-3.5/Modules/FindPackageMessage.cmake
           cmake-3.5/Modules/SelectLibraryConfigurations.cmake
           deprecated/YarpCMakeWorkarounds.cmake)
  if(EXISTS "${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_f}")
    install(CODE
 "if(EXISTS \"${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_f}\")
    message(STATUS \"Deleted: \\\"${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_f}\\\"\")
    file(REMOVE \"${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_f}\")
  endif()"
           COMPONENT scripts)
  endif()
endforeach()

# Remove no longer used directory from installation (since YARP 2.3.68)
foreach(_d cmake-3.1/  # since YARP 2.3.68
           cmake-3.5/
           ycm-0.2/    # since YARP 2.3.70
           ycm-0.3.1
           ycm-0.4.0)  # since YARP 2.3.72
  install(CODE
 "if(EXISTS \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_d}\")
    message(STATUS \"Deleted: \\\"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_d}\\\"\")
    file(REMOVE_RECURSE \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_d}\")
  endif()"
           COMPONENT scripts)
endforeach()
