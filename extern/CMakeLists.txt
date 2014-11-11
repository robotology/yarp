# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Do not build extern stuff with experimental warnings
if(YARP_EXPERIMENTAL_WARNINGS)
    string(REPLACE "${EXPERIMENTAL_WARNING_FLAGS} " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

if(CREATE_YARPSCOPE OR CREATE_GYARPMANAGER OR CREATE_YMANAGER)
    # TinyXML
    if(YARP_USE_TINYXML AND NOT YARP_USE_SYSTEM_TINYXML)
        add_subdirectory(tinyxml)
        set(TinyXML_INCLUDE_DIRS ${TinyXML_INCLUDE_DIRS} PARENT_SCOPE)
        set(TinyXML_LIBRARIES ${TinyXML_LIBRARIES} PARENT_SCOPE)
        set(TinyXML_DEFINITIONS ${TinyXML_DEFINITIONS} PARENT_SCOPE)
    endif()
endif()

# SQLite
if(YARP_USE_SQLITE AND NOT YARP_USE_SYSTEM_SQLITE)
    add_subdirectory(sqlite)
    set(SQLite_INCLUDE_DIRS ${SQLite_INCLUDE_DIRS} PARENT_SCOPE)
    set(SQLite_LIBRARIES ${SQLite_LIBRARIES} PARENT_SCOPE)
    set(SQLite_DEFINITIONS ${SQLite_DEFINITIONS} PARENT_SCOPE)
endif()

if(CREATE_YARPSCOPE AND YARP_HAS_GTK2)
    # GtkDatabox
    if(YARP_USE_GTKDATABOX AND NOT YARP_USE_SYSTEM_GTKDATABOX)
        add_subdirectory(gtkdatabox)
        set(GtkDatabox_INCLUDE_DIRS ${GtkDatabox_INCLUDE_DIRS} PARENT_SCOPE)
        set(GtkDatabox_LIBRARIES ${GtkDatabox_LIBRARIES} PARENT_SCOPE)
        set(GtkDatabox_DEFINITIONS ${GtkDatabox_DEFINITIONS} PARENT_SCOPE)
    endif()

    # GtkDataboxMM
    if(YARP_USE_GTKDATABOXMM AND NOT YARP_USE_SYSTEM_GTKDATABOXMM)
        add_subdirectory(gtkdataboxmm)
        set(GtkDataboxMM_INCLUDE_DIRS ${GtkDataboxMM_INCLUDE_DIRS} PARENT_SCOPE)
        set(GtkDataboxMM_LIBRARIES ${GtkDataboxMM_LIBRARIES} PARENT_SCOPE)
        set(GtkDataboxMM_DEFINITIONS ${GtkDataboxMM_DEFINITIONS} PARENT_SCOPE)
    endif()
endif()

if(CREATE_YARPBUILDER AND YARP_HAS_GTK2)
    # GooCanvas
    if(YARP_USE_GOOCANVAS AND NOT YARP_USE_SYSTEM_GOOCANVAS)
        add_subdirectory(goocanvas)
        set(GooCanvas_INCLUDE_DIRS ${GooCanvas_INCLUDE_DIRS} PARENT_SCOPE)
        set(GooCanvas_LIBRARIES ${GooCanvas_LIBRARIES} PARENT_SCOPE)
        set(GooCanvas_DEFINITIONS ${GooCanvas_DEFINITIONS} PARENT_SCOPE)
    endif()

    # GooCanvasMM
    if(YARP_USE_GOOCANVASMM AND NOT YARP_USE_SYSTEM_GOOCANVASMM)
        add_subdirectory(goocanvasmm)
        set(GooCanvasMM_INCLUDE_DIRS ${GooCanvasMM_INCLUDE_DIRS} PARENT_SCOPE)
        set(GooCanvasMM_LIBRARIES ${GooCanvasMM_LIBRARIES} PARENT_SCOPE)
        set(GooCanvasMM_DEFINITIONS ${GooCanvasMM_DEFINITIONS} PARENT_SCOPE)
    endif()
endif()
