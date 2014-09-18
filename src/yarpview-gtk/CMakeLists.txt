# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


if(CREATE_YARPVIEW AND YARP_HAS_GTK2)

  include_directories(${GTK2_INCLUDE_DIRS})
  add_definitions(${GTK2_DEFINITIONS})

  get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_TREE_INCLUDE_DIRS})

  if(YARP_HAS_ACE)
    include_directories(${ACE_INCLUDE_DIRS})
  endif(YARP_HAS_ACE)

  file(GLOB folder_source *.cpp)
  file(GLOB folder_header *.h)
  source_group("Source Files" FILES ${folder_source})
  source_group("Header Files" FILES ${folder_header})

  if(WIN32 AND NOT CYGWIN)
    add_definitions(-DYARP_WIN32_NOCONSOLE)
    add_executable(yarpview-gtk WIN32 ${folder_source} ${folder_header})
  else(WIN32 AND NOT CYGWIN)
    add_executable(yarpview-gtk ${folder_source} ${folder_header})
  endif(WIN32 AND NOT CYGWIN)

  target_link_libraries(yarpview-gtk YARP_OS
                                     YARP_sig
                                     YARP_init
                                     ${ACE_LIBRARIES}
                                     ${GTK2_GTK_LIBRARY}
                                     ${GTK2_GLIB_LIBRARY}
                                     ${GTK2_GDK_LIBRARY}
                                     ${GTK2_GDK_PIXBUF_LIBRARY}
                                     ${GTK2_GOBJECT_LIBRARY}
                                     ${GTK2_GTHREAD_LIBRARY})

  install(TARGETS yarpview-gtk
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})

  if(NOT TARGET yarpview-qt OR YARP_DEFAULT_GTK)
    set_target_properties(yarpview-gtk PROPERTIES OUTPUT_NAME yarpview)
  endif()

  yarp_install(FILES yarpview.xml
               DESTINATION ${YARP_MODULES_INSTALL_DIR})

endif()
