# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


project(yarpscope)

if(CREATE_YARPSCOPE)
    get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
    include_directories(${YARP_TREE_INCLUDE_DIRS})

    if(YARP_USE_GTK2)
        include_directories(${GTK2_INCLUDE_DIRS})
    else(YARP_USE_GTK2)
        include_directories (${GtkMM_INCLUDE_DIRS})
    endif(YARP_USE_GTK2)

    include_directories(${GtkDataboxMM_INCLUDE_DIRS})
    if(LINUX)
        add_definitions("--ansi --pedantic")
    endif(LINUX)

    add_subdirectory(src)
    
    configure_file(yarpscope.xml ${CMAKE_BINARY_DIR}/modules/yarpscope.xml COPYONLY)
    if(YARP_TREE_INCLUDE_DIRS)
        install(FILES ${CMAKE_BINARY_DIR}/modules/yarpscope.xml
                DESTINATION share/yarp/modules)
    endif(YARP_TREE_INCLUDE_DIRS)
endif(CREATE_YARPSCOPE)
