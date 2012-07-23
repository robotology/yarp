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

# GtkDatabox
if(YARP_USE_GtkDatabox AND NOT YARP_USE_SYSTEM_GtkDatabox)
    add_subdirectory(gtkdatabox)
endif(YARP_USE_GtkDatabox AND NOT YARP_USE_SYSTEM_GtkDatabox)

# GtkDataboxMM
if(YARP_USE_GtkDataboxMM AND NOT YARP_USE_SYSTEM_GtkDataboxMM)
    add_subdirectory(gtkdataboxmm)
endif(YARP_USE_GtkDataboxMM AND NOT YARP_USE_SYSTEM_GtkDataboxMM)
