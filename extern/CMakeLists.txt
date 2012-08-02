# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


# TinyXML
if(YARP_USE_TINYXML AND NOT YARP_USE_SYSTEM_TINYXML)
    add_subdirectory(tinyxml)
endif(YARP_USE_TINYXML AND NOT YARP_USE_SYSTEM_TINYXML)

# SQLite
if(YARP_USE_SQLITE AND NOT YARP_USE_SYSTEM_SQLITE)
    add_subdirectory(sqlite)
endif(YARP_USE_SQLITE AND NOT YARP_USE_SYSTEM_SQLITE)

# GtkDatabox
if(YARP_USE_GTKDATABOX AND NOT YARP_USE_SYSTEM_GTKDATABOX)
    add_subdirectory(gtkdatabox)
endif(YARP_USE_GTKDATABOX AND NOT YARP_USE_SYSTEM_GTKDATABOX)

# GtkDataboxMM
if(YARP_USE_GTKDATABOXMM AND NOT YARP_USE_SYSTEM_GTKDATABOXMM)
    add_subdirectory(gtkdataboxmm)
endif(YARP_USE_GTKDATABOXMM AND NOT YARP_USE_SYSTEM_GTKDATABOXMM)
