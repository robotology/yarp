# Try to find the GtkDatabox library
# GtkDatabox_FOUND - system has GtkDatabox
# GtkDatabox_INCLUDE_DIRS - GtkDatabox include directory
# GtkDatabox_LIBRARY_DIRS - GtkDatabox library directory
# GtkDatabox_LIBRARIES - GtkDatabox libraries

# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


if(NOT WIN32)
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        if(GtkDatabox_FIND_VERSION)
            if(GtkDatabox_FIND_VERSION_EXACT)
                pkg_check_modules(PC_GTKDATABOX QUIET gtkdatabox=${GTKDATABOX_FIND_VERSION})
            else(GtkDatabox_FIND_VERSION_EXACT)
                pkg_check_modules(PC_GTKDATABOX QUIET gtkdatabox>=${GTKDATABOX_FIND_VERSION})
            endif(GtkDatabox_FIND_VERSION_EXACT)
        else(GtkDatabox_FIND_VERSION)
            pkg_check_modules(PC_GTKDATABOX QUIET gtkdatabox)
        endif(GtkDatabox_FIND_VERSION)
    endif(PKG_CONFIG_FOUND)
endif(NOT WIN32)

set(GtkDatabox_INCLUDE_DIRS ${PC_GTKDATABOX_INCLUDE_DIRS} CACHE PATH "GtkDatabox include directory" FORCE)
set(GtkDatabox_LIBRARY_DIRS ${PC_GTKDATABOX_LIBRARY_DIRS} CACHE PATH "GtkDatabox library directory" FORCE)
set(GtkDatabox_LIBRARIES ${PC_GTKDATABOX_LIBRARIES} CACHE STRING "GtkDatabox libraries" FORCE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GtkDatabox
                                  DEFAULT_MSG
                                  GtkDatabox_LIBRARIES
)

set(GtkDatabox_FOUND ${GTKDATABOX_FOUND})

mark_as_advanced(GtkDatabox_INCLUDE_DIRS GtkDatabox_LIBRARY_DIRS GtkDatabox_LIBRARIES)
