# Copyright: (C) 2009, 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Cannot APPEND here due to CMake 2.6.0 bug
#set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS 
#  ${CMAKE_BINARY_DIR}/generated_include)
set_property(GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS 
  ${CMAKE_BINARY_DIR}/generated_include)

foreach(conf version system options api)
  set(outconf ${CMAKE_BINARY_DIR}/generated_include/yarp/conf/${conf}.h)
  configure_file(${CMAKE_SOURCE_DIR}/conf/template/yarp_config_${conf}.h.in
                 ${outconf} IMMEDIATE)
  message(STATUS "generated ${outconf}")
  install(FILES ${outconf} COMPONENT development DESTINATION include/yarp/conf)
endforeach(conf)

# There are several macros we traditionally make available to
# users of YARP.

foreach(conf YarpPlugin.cmake YarpDevice.cmake FindOpenCV.cmake FindGtkPlus.cmake FindGSL.cmake FindPortAudio.cmake FindGtkWin32.cmake FindFFMPEG.cmake FindBoost.cmake FindACE.cmake FindGthread.cmake)
  #configure_file(${CMAKE_SOURCE_DIR}/conf/${conf}
  #               ${CMAKE_BINARY_DIR}/conf/${conf} COPYONLY IMMEDIATE)
  install(FILES ${CMAKE_SOURCE_DIR}/conf/${conf} COMPONENT scripts 
	  DESTINATION share/yarp/cmake)
endforeach()

foreach(templ yarp_plugin_carrier.cpp.in yarp_plugin_device.cpp.in yarpdev_lib.cpp.in yarpdev_lib.h.in yarpdev_lib_main.cpp.in)
  #configure_file(${CMAKE_SOURCE_DIR}/conf/template/${conf}
  #               ${CMAKE_BINARY_DIR}/conf/template/${conf} COPYONLY IMMEDIATE)
  install(FILES ${CMAKE_SOURCE_DIR}/conf/template/${templ} COMPONENT scripts 
	  DESTINATION share/yarp/cmake/template)
endforeach()

