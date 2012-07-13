# Try to find the GtkDatabox library
# GTKDATABOX_FOUND - system has GtkDatabox
# GTKDATABOX_INCLUDE_DIRS - GtkDatabox include directory
# GTKDATABOX_LIBRARY_DIRS - GtkDatabox library directory
# GTKDATABOX_LIBRARIES - GtkDatabox libraries

# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


set(GTKDATABOX_FIND_REQUIRED ${GtkDatabox_FIND_REQUIRED})
if(GTKDATABOX_INCLUDE_DIR AND GTKDATABOX_LIBRARIES)
    set(GTKDATABOX_FIND_QUIETLY TRUE)
endif(GTKDATABOX_INCLUDE_DIR AND GTKDATABOX_LIBRARIES)


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

set(GTKDATABOX_INCLUDE_DIRS ${PC_GTKDATABOX_INCLUDE_DIRS} CACHE PATH "GtkDatabox include directory")
set(GTKDATABOX_LIBRARY_DIRS ${PC_GTKDATABOX_LIBRARY_DIRS} CACHE PATH "GtkDatabox library directory")
set(GTKDATABOX_LIBRARIES ${PC_GTKDATABOX_LIBRARIES} CACHE STRING "GtkDatabox libraries")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GtkDatabox
                                  DEFAULT_MSG
                                  GTKDATABOX_INCLUDE_DIRS
                                  GTKDATABOX_LIBRARIES
)

mark_as_advanced(GTKDATABOX_INCLUDE_DIRS GTKDATABOX_LIBRARY_DIRS GTKDATABOX_LIBRARIES)
