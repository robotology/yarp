# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

# YARP CMake Modules
set(YARP_CMAKE_FILES YarpPlugin.cmake
                     YarpIDL.cmake
                     YarpInstallationHelpers.cmake
                     YarpPrintFeature.cmake)
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
                                ycm-${YCM_REQUIRED_VERSION}/modules/InstallBasicPackageFiles.cmake
                                ycm-${YCM_REQUIRED_VERSION}/modules/ReplaceImportedTargets.cmake
                                ycm-${YCM_REQUIRED_VERSION}/modules/StandardFindModule.cmake)
    set(YCM_CMAKE_3rdparty_FILES ycm-${YCM_REQUIRED_VERSION}/3rdparty/FindEigen3.cmake
                                 ycm-${YCM_REQUIRED_VERSION}/3rdparty/FindFFMPEG.cmake
                                 ycm-${YCM_REQUIRED_VERSION}/3rdparty/FindOctave.cmake
                                 ycm-${YCM_REQUIRED_VERSION}/3rdparty/FindQCustomPlot.cmake)
    set(YCM_CMAKE_cmake_3.8_FILES ycm-${YCM_REQUIRED_VERSION}/cmake-3.8/Modules/CMakeParseArguments.cmake
                                  ycm-${YCM_REQUIRED_VERSION}/cmake-3.8/Modules/FindPackageHandleStandardArgs.cmake
                                  ycm-${YCM_REQUIRED_VERSION}/cmake-3.8/Modules/FindPackageMessage.cmake
                                  ycm-${YCM_REQUIRED_VERSION}/cmake-3.8/Modules/FindSWIG.cmake
                                  ycm-${YCM_REQUIRED_VERSION}/cmake-3.8/Modules/UseSWIG.cmake)
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
    if(${CMAKE_VERSION} VERSION_LESS 3.8)
      # Files from CMake 3.8, available only if CMake on the system is older
      install(FILES ${YCM_CMAKE_cmake_3.8_FILES}
              COMPONENT scripts
              DESTINATION share/yarp/cmake/ycm-${YCM_REQUIRED_VERSION}/cmake-next/Modules)
    endif()
  endif()
endif()

# Remove renamed files from installation
foreach(_f YarpDeprecatedWarning.cmake)                           # since YARP 3.0.0
  if(EXISTS "${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_f}")
    install(CODE
 "if(EXISTS \"${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_f}\")
    message(STATUS \"Deleted: \\\"${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_f}\\\"\")
    file(REMOVE \"${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_f}\")
  endif()"
           COMPONENT scripts)
  endif()
endforeach()

# Remove no longer used directory from installation
foreach(_d ycm-0.6.0   # since YARP 3.0.0
           ycm-0.7.0)
  install(CODE
 "if(EXISTS \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_d}\")
    message(STATUS \"Deleted: \\\"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_d}\\\"\")
    file(REMOVE_RECURSE \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/yarp/cmake/${_d}\")
  endif()"
           COMPONENT scripts)
endforeach()
