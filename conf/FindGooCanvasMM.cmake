# Try to find the GooCanvasMM library
# GooCanvasMM_FOUND - system has GooCanvasMM
# GooCanvasMM_INCLUDE_DIRS - GooCanvasMM include directory
# GooCanvasMM_LIBRARY_DIRS - GooCanvasMM library directory
# GooCanvasMM_LIBRARIES - GooCanvasMM libraries

# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


if(NOT WIN32)
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        if(GooCanvasMM_FIND_VERSION)
            if(GooCanvasMM_FIND_VERSION_EXACT)
                pkg_check_modules(PC_GOOCANVASMM QUIET goocanvasmm-1.0=${GooCanvasMM_FIND_VERSION})
            else(GooCanvasMM_FIND_VERSION_EXACT)
                pkg_check_modules(PC_GOOCANVASMM QUIET goocanvasmm-1.0>=${GooCanvasMM_FIND_VERSION})
            endif(GooCanvasMM_FIND_VERSION_EXACT)
        else(GooCanvasMM_FIND_VERSION)
            pkg_check_modules(PC_GOOCANVASMM QUIET goocanvasmm-1.0)
        endif(GooCanvasMM_FIND_VERSION)
    endif(PKG_CONFIG_FOUND)
endif(NOT WIN32)

set(GooCanvasMM_INCLUDE_DIRS ${PC_GOOCANVASMM_INCLUDE_DIRS} CACHE PATH "GooCanvasMM include directory" FORCE)
set(GooCanvasMM_LIBRARY_DIRS ${PC_GOOCANVASMM_LIBRARY_DIRS} CACHE PATH "GooCanvasMM library directory" FORCE)
set(GooCanvasMM_LIBRARIES ${PC_GOOCANVASMM_LIBRARIES} CACHE STRING "GooCanvasMM libraries" FORCE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GooCanvasMM
                                  DEFAULT_MSG
                                  GooCanvasMM_LIBRARIES
)

set(GooCanvasMM_FOUND ${GOOCANVASMM_FOUND})

mark_as_advanced(GooCanvasMM_INCLUDE_DIRS GooCanvasMM_LIBRARY_DIRS GooCanvasMM_LIBRARIES)
