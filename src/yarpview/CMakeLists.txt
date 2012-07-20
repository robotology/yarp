# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

option(CREATE_GUIS "Do you want to compile GUIs" OFF)

if (CREATE_GUIS)

  OPTION(YARP_USE_GTK2 FALSE "Use official CMake GTK2 find scripts, rather than homebrewed scripts")
  IF (YARP_USE_GTK2)
    find_package(GTK2 2.8 COMPONENTS gtk gtkmm)
    FIND_PACKAGE(Gthread REQUIRED)
    if(GTK2_FOUND)
      include_directories(${GTK2_INCLUDE_DIRS})
      link_libraries(${GTK2_LIBRARIES})
      #set up gthread
      ADD_DEFINITIONS(${GTHREAD_C_FLAGS})
      LINK_LIBRARIES(${Gthread_LIBRARIES})
      INCLUDE_DIRECTORIES(${Gthread_INCLUDE_DIRS})
    endif()
  ELSE ()
    FIND_PACKAGE(GtkPlus REQUIRED)
    FIND_PACKAGE(Gthread REQUIRED)
    # set up to use GTK+
    #ADD_DEFINITIONS(${GTKPLUS_C_FLAGS})
    LINK_LIBRARIES(${GtkPlus_LIBRARIES})
    #LINK_DIRECTORIES(${GTKPLUS_LIBRARY_DIR})
    INCLUDE_DIRECTORIES(${GtkPlus_INCLUDE_DIRS})
    
    #set up gthread
    ADD_DEFINITIONS(${GTHREAD_C_FLAGS})
    LINK_LIBRARIES(${Gthread_LIBRARIES})
    INCLUDE_DIRECTORIES(${Gthread_INCLUDE_DIRS})

  ENDIF ()


  GET_PROPERTY(YARP_TREE_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  INCLUDE_DIRECTORIES(${YARP_TREE_INCLUDE_DIRS})

  IF (YARP_HAS_ACE)
    INCLUDE_DIRECTORIES(${ACE_INCLUDE_DIRS})
  ENDIF (YARP_HAS_ACE)

  FILE(GLOB folder_source *.cpp)
  FILE(GLOB folder_header *.h)
  SOURCE_GROUP("Source Files" FILES ${folder_source})
  SOURCE_GROUP("Header Files" FILES ${folder_header})

  IF (WIN32 AND NOT CYGWIN)
    ADD_DEFINITIONS(-DYARP_WIN32_NOCONSOLE)
    ADD_EXECUTABLE(yarpview WIN32 ${folder_source} ${folder_header})
  ELSE (WIN32 AND NOT CYGWIN)
    ADD_EXECUTABLE(yarpview ${folder_source} ${folder_header})
  ENDIF (WIN32 AND NOT CYGWIN)

  TARGET_LINK_LIBRARIES(yarpview YARP_sig YARP_init)

  INSTALL(TARGETS yarpview COMPONENT utilities DESTINATION bin)

endif(CREATE_GUIS)
