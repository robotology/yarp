# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

find_program(DOXYGEN_EXE NAMES doxygen)
mark_as_advanced(DOXYGEN_EXE)
if (DOXYGEN_EXE)
  set(DOX_GENERATE_XML NO)
  option(YARP_DOXYGEN_XML "Generate doxygen XML output, for use by SWIG" NO)
  mark_as_advanced(YARP_DOXYGEN_XML)
  option(YARP_DOXYGEN_VERBOSE "Generate verbose doxygen output" NO)
  mark_as_advanced(YARP_DOXYGEN_VERBOSE)
  if (YARP_DOXYGEN_XML)
    set(DOX_GENERATE_XML YES)
  endif ()
  set(DOX_FILE Doxyfile.part)
  if (YARP_DOXYGEN_VERBOSE)
    set(DOX_EXTRA_DIR src conf doc/spec)
    set(DOX_EXTRA_PATTERNS "*.cpp")
  endif ()
  configure_file(${CMAKE_SOURCE_DIR}/conf/doxygen/${DOX_FILE}.template
                 ${CMAKE_BINARY_DIR}/dox/${DOX_FILE} IMMEDIATE)
  add_custom_target(dox COMMAND ${DOXYGEN_EXE} ${CMAKE_BINARY_DIR}/dox/${DOX_FILE})
endif ()
