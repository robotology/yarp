# Try to find the GooCanvas library
# GooCanvas_FOUND - system has GooCanvas
# GooCanvas_INCLUDE_DIRS - GooCanvas include directory
# GooCanvas_LIBRARY_DIRS - GooCanvas library directory
# GooCanvas_LIBRARIES - GooCanvas libraries

# Copyright (C) 2013  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


if(NOT WIN32)
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        if(GooCanvas_FIND_VERSION)
            if(GooCanvas_FIND_VERSION_EXACT)
                pkg_check_modules(PC_GOOCANVAS QUIET goocanvas=${GooCanvas_FIND_VERSION})
            else(GooCanvas_FIND_VERSION_EXACT)
                pkg_check_modules(PC_GOOCANVAS QUIET goocanvas>=${GooCanvas_FIND_VERSION})
            endif(GooCanvas_FIND_VERSION_EXACT)
        else(GooCanvas_FIND_VERSION)
            pkg_check_modules(PC_GOOCANVAS QUIET goocanvas)
        endif(GooCanvas_FIND_VERSION)
    endif(PKG_CONFIG_FOUND)
endif(NOT WIN32)

set(GooCanvas_INCLUDE_DIRS ${PC_GOOCANVAS_INCLUDE_DIRS} CACHE PATH "GooCanvas include directory" FORCE)
set(GooCanvas_LIBRARY_DIRS ${PC_GOOCANVAS_LIBRARY_DIRS} CACHE PATH "GooCanvas library directory" FORCE)
set(GooCanvas_LIBRARIES ${PC_GOOCANVAS_LIBRARIES} CACHE STRING "GooCanvas libraries" FORCE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GooCanvas
                                  DEFAULT_MSG
                                  GooCanvas_LIBRARIES
)

set(GooCanvas_FOUND ${GOOCANVAS_FOUND})

mark_as_advanced(GooCanvas_INCLUDE_DIRS GooCanvas_LIBRARY_DIRS GooCanvas_LIBRARIES)
