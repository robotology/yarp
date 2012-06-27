# Try to find the GtkDatabox library
# GTKDATABOX_FOUND - system has GtkDatabox
# GTKDATABOX_INCLUDE_DIRS - Include those to use GtkDatabox
# GTKDATABOX_LIBRARIES - Link these to use GtkDatabox

# Copyright (c) 2012, Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


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

set(GTKDATABOX_INCLUDE_DIRS ${PC_GTKDATABOX_INCLUDE_DIRS})
set(GTKDATABOX_LIBRARIES ${PC_GTKDATABOX_LIBRARIES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GtkDatabox DEFAULT_MSG
                                  GTKDATABOX_LIBRARIES GTKDATABOX_INCLUDE_DIRS)

