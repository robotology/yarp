# Copyright: (C) 2009, 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

set_property(GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/generated_include)

foreach(conf version system options api)
    set(outconf ${CMAKE_BINARY_DIR}/generated_include/yarp/conf/${conf}.h)
    configure_file(${CMAKE_SOURCE_DIR}/conf/template/yarp_config_${conf}.h.in
                   ${outconf} IMMEDIATE)
    message(STATUS "generated ${outconf}")
    install(FILES ${outconf} COMPONENT development DESTINATION include/yarp/conf)
endforeach(conf)

# There are several macros we traditionally make available to
# users of YARP.

foreach(conf YarpPlugin.cmake
             YarpIDL.cmake
             YarpDevice.cmake
             YarpDeprecatedWarning.cmake
             FindACE.cmake
             FindAtlas.cmake
             FindFFMPEG.cmake
             FindGSL.cmake
             FindGthread.cmake
             FindGtkDatabox.cmake
             FindGtkDataboxMM.cmake
             FindGtkMM.cmake
             FindGtkMMUnix.cmake
             FindGtkMMWin32.cmake
             FindGtkPlus.cmake
             FindGtkWin32.cmake
             FindOpenCV.cmake
             FindPortAudio.cmake
             FindSQLite.cmake
             FindTinyXML.cmake
         )
    install(FILES ${CMAKE_SOURCE_DIR}/conf/${conf}
            COMPONENT scripts
            DESTINATION share/yarp/cmake)
endforeach(conf)

if(${CMAKE_VERSION} VERSION_LESS 2.8.11.2)
  foreach(conf cmake-2.8.11.2/Modules/FindGTK2.cmake
               cmake-2.8.11.2/Modules/FindPackageHandleStandardArgs.cmake
               cmake-2.8.11.2/Modules/FindPackageMessage.cmake
               cmake-2.8.11.2/Modules/CMakeParseArguments.cmake
               cmake-2.8.11.2/Modules/FindFreetype.cmake
         )
      install(FILES ${CMAKE_SOURCE_DIR}/conf/${conf}
              COMPONENT scripts
              DESTINATION share/yarp/cmake/cmake-2.8.11.2/Modules)
  endforeach(conf)
endif()

foreach(templ yarp_plugin_carrier.cpp.in
              yarp_plugin_device.cpp.in
              yarpdev_lib.cpp.in
              yarpdev_lib.h.in
              yarpdev_lib_main.cpp.in
              YarpTweakIDL.cmake.in
              placeGeneratedYarpIdlFiles.cmake.in)
    install(FILES ${CMAKE_SOURCE_DIR}/conf/template/${templ}
            COMPONENT scripts
            DESTINATION share/yarp/cmake/template)
endforeach(templ)
