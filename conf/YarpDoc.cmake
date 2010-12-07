# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

find_program(DOXYGEN_EXE NAMES doxygen)
mark_as_advanced(DOXYGEN_EXE)
if (DOXYGEN_EXE)
  SET(DOX_GENERATE_XML NO)
  OPTION(YARP_DOXYGEN_XML "Generate doxygen XML output, for use by SWIG" NO)
  MARK_AS_ADVANCED(YARP_DOXYGEN_XML)
  IF(YARP_DOXYGEN_XML)
    SET(DOX_GENERATE_XML YES)
  ENDIF(YARP_DOXYGEN_XML)
  configure_file(${CMAKE_SOURCE_DIR}/conf/doxygen/Doxyfile.part.template
                 ${CMAKE_BINARY_DIR}/dox/Doxyfile.part IMMEDIATE)
  add_custom_target(dox COMMAND ${DOXYGEN_EXE} ${CMAKE_BINARY_DIR}/dox/Doxyfile.part)
endif ()
