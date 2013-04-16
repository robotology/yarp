# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


if(CREATE_GUIS)

    if(YARP_USE_GTK2)
        include_directories(${GTK2_INCLUDE_DIRS})
    else(YARP_USE_GTK2)
        include_directories(${GtkPlus_INCLUDE_DIRS})
    endif(YARP_USE_GTK2)

    include_directories(${Gthread_INCLUDE_DIRS})
    add_definitions(${GTHREAD_C_FLAGS})

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
        add_executable(yarpview WIN32 ${folder_source} ${folder_header})
    else(WIN32 AND NOT CYGWIN)
        add_executable(yarpview ${folder_source} ${folder_header})
    endif(WIN32 AND NOT CYGWIN)

    target_link_libraries(yarpview YARP_sig YARP_init)
    target_link_libraries(yarpview ${ACE_LIBRARIES})

    if(YARP_USE_GTK2)
        target_link_libraries(yarpview ${GTK2_GTK_LIBRARY}
                                       ${GTK2_GLIB_LIBRARY}
                                       ${GTK2_GDK_LIBRARY}
                                       ${GTK2_GDK_PIXBUF_LIBRARY}
                                       ${GTK2_GOBJECT_LIBRARY}
        )
    else(YARP_USE_GTK2)
        target_link_libraries(yarpview ${GtkPlus_LIBRARIES})
    endif(YARP_USE_GTK2)

    target_link_libraries(yarpview ${Gthread_LIBRARIES})

    install(TARGETS yarpview COMPONENT utilities DESTINATION bin)

    configure_file(yarpview.xml ${CMAKE_BINARY_DIR}/modules/yarpview.xml COPYONLY)
    if(YARP_TREE_INCLUDE_DIRS)
        install(FILES ${CMAKE_BINARY_DIR}/modules/yarpview.xml
                DESTINATION share/yarp/modules)
    endif(YARP_TREE_INCLUDE_DIRS)


endif(CREATE_GUIS)
