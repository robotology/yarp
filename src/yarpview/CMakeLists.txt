# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

option(CREATE_GUIS "Do you want to compile GUIs" OFF)

if (CREATE_GUIS)

  FIND_PACKAGE(GtkPlus)
  IF (NOT GtkPlus_FOUND)
	MESSAGE(STATUS " gtk+ not found, won't compile dependent tools")
  ENDIF (NOT GtkPlus_FOUND)

  FIND_PACKAGE(Gthread)
  IF (NOT Gthread_FOUND)
	MESSAGE(STATUS " gthread not found, won't compile dependent tools")
  ENDIF(NOT Gthread_FOUND)

  # set up to use GTK+
  ADD_DEFINITIONS(${GTKPLUS_C_FLAGS})
  LINK_LIBRARIES(${GTKPLUS_LINK_FLAGS})
  LINK_DIRECTORIES(${GTKPLUS_LIBRARY_DIR})
  INCLUDE_DIRECTORIES(${GTKPLUS_INCLUDE_DIR})

  #set up gthread
  ADD_DEFINITIONS(${GTHREAD_C_FLAGS})
  LINK_LIBRARIES(${GTHREAD_LINK_FLAGS})
  INCLUDE_DIRECTORIES(${GTHREAD_INCLUDE_DIR})

  GET_PROPERTY(YARP_TREE_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  INCLUDE_DIRECTORIES(${YARP_TREE_INCLUDE_DIRS})

  FILE(GLOB folder_source *.cpp)
  FILE(GLOB folder_header *.h)
  SOURCE_GROUP("Source Files" FILES ${folder_source})
  SOURCE_GROUP("Header Files" FILES ${folder_header})

  IF (WIN32 AND NOT CYGWIN)
    ADD_DEFINITION(-DYARP_WIN32_NOCONSOLE)
    ADD_EXECUTABLE(yarpview WIN32 ${folder_source} ${folder_header})
  ELSE (WIN32 AND NOT CYGWIN)
    ADD_EXECUTABLE(yarpview ${folder_source} ${folder_header})
  ENDIF (WIN32 AND NOT CYGWIN)

  TARGET_LINK_LIBRARIES(yarpview YARP_sig YARP_init)

  INSTALL(TARGETS yarpview COMPONENT utilities DESTINATION bin)

endif(CREATE_GUIS)
