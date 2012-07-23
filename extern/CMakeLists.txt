# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


# TinyXML
if(YARP_USE_TinyXML AND NOT YARP_USE_SYSTEM_TinyXML)
    add_subdirectory(tinyxml)
endif(YARP_USE_TinyXML AND NOT YARP_USE_SYSTEM_TinyXML)

# Sqlite
if(YARP_USE_SQLite AND NOT YARP_USE_SYSTEM_SQLite)
    add_subdirectory(sqlite)
endif(YARP_USE_SQLite AND NOT YARP_USE_SYSTEM_SQLite)


if(CREATE_GUIS)
    # Gtk2 and GtkMM

    option(YARP_USE_GTK2 FALSE "Use official CMake GTK2 find scripts, rather than homebrewed scripts")

    if(YARP_USE_GTK2)
        find_package(GTK2 2.8 COMPONENTS gtk gtkmm)
    else(YARP_USE_GTK2)
        find_package(GtkPlus REQUIRED)
        find_package(GtkMM REQUIRED)
    endif(YARP_USE_GTK2)

    if (CREATE_YARPSCOPE)
        # GtkDatabox
        option(YARP_USE_SYSTEM_GTKDATABOX "Use system GtkDatabox library" OFF)

        if(YARP_USE_SYSTEM_GTKDATABOX)
            find_package(GtkDatabox REQUIRED)
        else(YARP_USE_SYSTEM_GTKDATABOX)
            add_subdirectory(gtkdatabox)
        endif(YARP_USE_SYSTEM_GTKDATABOX)


        # GtkDataboxMM
        option(YARP_USE_SYSTEM_GTKDATABOXMM "Use system GtkDataboxMM library" OFF)

        if(YARP_USE_SYSTEM_GTKDATABOXMM)
            find_package(GtkDataboxMM REQUIRED)
        else(YARP_USE_SYSTEM_GTKDATABOXMM)
            add_subdirectory(gtkdataboxmm)
        endif(YARP_USE_SYSTEM_GTKDATABOXMM)
    endif (CREATE_YARPSCOPE)
endif(CREATE_GUIS)
