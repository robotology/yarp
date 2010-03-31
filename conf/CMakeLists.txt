# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET_PROPERTY(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS 
  ${CMAKE_BINARY_DIR}/generated_include)

FOREACH(conf version system options)
  SET(outconf ${CMAKE_BINARY_DIR}/generated_include/yarp/conf/${conf}.h)
  CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/conf/template/yarp_config_${conf}.h.in
                 ${outconf} IMMEDIATE)
  MESSAGE(STATUS "generated ${outconf}")
  INSTALL(FILES ${outconf} COMPONENT development DESTINATION include/yarp/conf)
ENDFOREACH(conf)

# There are several macros we would like to have available
# for user

FOREACH(conf YarpDevice.cmake YarpModuleHooks.cmake FindOpenCV.cmake FindGtkPlus.cmake FindGSL.cmake FindPortAudio.cmake FindGtkWin32.cmake FindFFMPEG.cmake FindBoost.cmake FindAce.cmake)
  CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/conf/${conf}
                 ${CMAKE_BINARY_DIR}/conf/${conf} COPYONLY IMMEDIATE)
ENDFOREACH(conf)

