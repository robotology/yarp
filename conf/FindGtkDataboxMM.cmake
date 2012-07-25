# Try to find the GtkDataboxMM library
# GtkDataboxMM_FOUND - system has GtkDataboxMM
# GtkDataboxMM_INCLUDE_DIRS - GtkDataboxMM include directory
# GtkDataboxMM_LIBRARY_DIRS - GtkDataboxMM library directory
# GtkDataboxMM_LIBRARIES - GtkDataboxMM libraries

# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


if(NOT WIN32)
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        if(GtkDataboxMM_FIND_VERSION)
            if(GtkDataboxMM_FIND_VERSION_EXACT)
                pkg_check_modules(PC_GTKDATABOXMM QUIET gtkdataboxmm-0.0=${GTKDATABOXMM_FIND_VERSION})
            else(GtkDataboxMM_FIND_VERSION_EXACT)
                pkg_check_modules(PC_GTKDATABOXMM QUIET gtkdataboxmm-0.0>=${GTKDATABOXMM_FIND_VERSION})
            endif(GtkDataboxMM_FIND_VERSION_EXACT)
        else(GtkDataboxMM_FIND_VERSION)
            pkg_check_modules(PC_GTKDATABOXMM QUIET gtkdataboxmm-0.0)
        endif(GtkDataboxMM_FIND_VERSION)
    endif(PKG_CONFIG_FOUND)
endif(NOT WIN32)

set(GtkDataboxMM_INCLUDE_DIRS ${PC_GTKDATABOXMM_INCLUDE_DIRS} CACHE PATH "GtkDataboxMM include directory" FORCE)
set(GtkDataboxMM_LIBRARY_DIRS ${PC_GTKDATABOXMM_LIBRARY_DIRS} CACHE PATH "GtkDataboxMM library directory" FORCE)
set(GtkDataboxMM_LIBRARIES ${PC_GTKDATABOXMM_LIBRARIES} CACHE STRING "GtkDataboxMM libraries" FORCE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GtkDataboxMM
                                  DEFAULT_MSG
                                  GtkDataboxMM_LIBRARIES
)

set(GtkDataboxMM_FOUND ${GTKDATABOXMM_FOUND})

mark_as_advanced(GtkDataboxMM_INCLUDE_DIRS GtkDataboxMM_LIBRARY_DIRS GtkDataboxMM_LIBRARIES)
