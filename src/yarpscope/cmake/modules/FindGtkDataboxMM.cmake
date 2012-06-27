# Try to find the GtkDataboxMM library
# GTKDATABOXMM_FOUND - system has GtkDataboxMM
# GTKDATABOXMM_INCLUDE_DIRS - GtkDataboxMM include directory
# GTKDATABOXMM_LIBRARY_DIRS - GtkDataboxMM library directory
# GTKDATABOXMM_LIBRARIES - GtkDataboxMM libraries

# Copyright (c) 2012, Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


set(GTKDATABOXMM_FIND_REQUIRED ${GtkDataboxMM_FIND_REQUIRED})
if(GTKDATABOXMM_INCLUDE_DIR AND GTKDATABOXMM_LIBRARIES)
    set(GTKDATABOXMM_FIND_QUIETLY TRUE)
endif(GTKDATABOXMM_INCLUDE_DIR AND GTKDATABOXMM_LIBRARIES)


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

set(GTKDATABOXMM_INCLUDE_DIRS ${PC_GTKDATABOXMM_INCLUDE_DIRS} CACHE PATH "GtkDataboxMM include directory")
set(GTKDATABOXMM_LIBRARY_DIRS ${PC_GTKDATABOXMM_LIBRARY_DIRS} CACHE PATH "GtkDataboxMM library directory")
set(GTKDATABOXMM_LIBRARIES ${PC_GTKDATABOXMM_LIBRARIES} CACHE STRING "GtkDataboxMM libraries")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GtkDataboxMM
                                  DEFAULT_MSG
                                  GTKDATABOXMM_INCLUDE_DIRS
                                  GTKDATABOXMM_LIBRARIES
)

mark_as_advanced(GTKDATABOXMM_INCLUDE_DIRS GTKDATABOXMM_LIBRARY_DIRS GTKDATABOXMM_LIBRARIES)
