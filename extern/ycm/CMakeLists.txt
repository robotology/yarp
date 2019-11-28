# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

set(YCM_REQUIRED_VERSION 0.11.0) # Used also by YarpFindDependencies
find_package(YCM ${YCM_REQUIRED_VERSION} QUIET)

if(NOT YCM_FOUND)

  set(YCM_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/ycm-${YCM_REQUIRED_VERSION}/find-modules"
                      "${CMAKE_CURRENT_LIST_DIR}/ycm-${YCM_REQUIRED_VERSION}/modules"
                      "${CMAKE_CURRENT_LIST_DIR}/ycm-${YCM_REQUIRED_VERSION}/3rdparty")
  if(${CMAKE_VERSION} VERSION_LESS 3.14)
    list(APPEND YCM_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/ycm-${YCM_REQUIRED_VERSION}/cmake-3.14/Modules")
  endif()


  if(NOT YARP_NO_DEPRECATED) # since YARP 2.3.68. Warning in YARP 3.0.1

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
      set(YCM_CMAKE_cmake_3.14_FILES ycm-${YCM_REQUIRED_VERSION}/cmake-3.14/Modules/CMakeParseArguments.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/cmake-3.14/Modules/FindPackageHandleStandardArgs.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/cmake-3.14/Modules/FindPackageMessage.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/cmake-3.14/Modules/FindSWIG.cmake
                                     ycm-${YCM_REQUIRED_VERSION}/cmake-3.14/Modules/UseSWIG.cmake)

      install(FILES ${YCM_CMAKE_find_modules_FILES}
              COMPONENT scripts
              DESTINATION share/yarp/cmake/ycm-${YCM_REQUIRED_VERSION}/find-modules)
      install(FILES ${YCM_CMAKE_modules_FILES}
              COMPONENT scripts
              DESTINATION share/yarp/cmake/ycm-${YCM_REQUIRED_VERSION}/modules)
      install(FILES ${YCM_CMAKE_3rdparty_FILES}
              COMPONENT scripts
              DESTINATION share/yarp/cmake/ycm-${YCM_REQUIRED_VERSION}/3rdparty)
      if(${CMAKE_VERSION} VERSION_LESS 3.14)
        # Files from CMake 3.14, available only if CMake on the system is older
        install(FILES ${YCM_CMAKE_cmake_3.14_FILES}
                COMPONENT scripts
                DESTINATION share/yarp/cmake/ycm-${YCM_REQUIRED_VERSION}/cmake-next/Modules)
      endif()
    endif()
  endif()
endif()

set(YCM_MODULE_PATH ${YCM_MODULE_PATH} PARENT_SCOPE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${YCM_MODULE_PATH} PARENT_SCOPE)
set(YCM_REQUIRED_VERSION ${YCM_REQUIRED_VERSION} PARENT_SCOPE) # Used also by YarpFindDependencies and YARPConfig
set(YCM_FOUND ${YCM_FOUND} PARENT_SCOPE)

# Remove no longer used directory from installation
include(YarpRemoveFile)
foreach(_d ycm-0.6.0   # since YARP 3.0.0
           ycm-0.7.0
           ycm-0.8.0
           ycm-0.8.1   # since YARP 3.0.1
           ycm-0.8.20180628.11
           ycm-0.8.20180712.5
           ycm-0.8.20180720.3
           ycm-0.9.0   # since YARP 3.1.1
           ycm-0.9.0.8
           ycm-0.9.20181128.10
           ycm-0.9.20181204.8
           ycm-0.9.20190213.9
           ycm-0.9.20190315.16
           ycm-0.9.20190329.27
           ycm-0.10.0
           ycm-0.10.1
           ycm-0.10.2
           ycm-0.10.3)
  yarp_remove_directory("share/yarp/cmake/${_d}")
endforeach()
