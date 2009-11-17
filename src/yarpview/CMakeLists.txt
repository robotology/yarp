# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
 
# set up to use GTK+
ADD_DEFINITIONS(${GTKPLUS_C_FLAGS})
LINK_LIBRARIES(${GTKPLUS_LINK_FLAGS})
INCLUDE_DIRECTORIES(${GTKPLUS_INCLUDE_DIR})

#set up gthread
ADD_DEFINITIONS(${GTHREAD_C_FLAGS})
LINK_LIBRARIES(${GTHREAD_LINK_FLAGS})
INCLUDE_DIRECTORIES(${GTHREAD_INCLUDE_DIR})

# set up to use libYARP_*
SET(NEED_LIBS YARP_OS YARP_sig)

IF (WIN32 AND NOT CYGWIN)
  YarpProgram(yarpview NO_CONSOLE)
  INSTALL_TARGETS(/bin yarpview)
ELSE (WIN32 AND NOT CYGWIN)
  YarpProgram(yarpview NO_CONSOLE)
  INSTALL_TARGETS(/bin yarpview)
ENDIF (WIN32 AND NOT CYGWIN)

