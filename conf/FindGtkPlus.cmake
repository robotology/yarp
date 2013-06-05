# Try to find GTK+ library
#
# On exit set:
# GtkPlus_FOUND
# GtkPlus_LIBRARIES
# GtkPlus_INCLUDE_DIRS
#
# Old variables:
#
# GTKPLUS_FOUND
# GTKPLUS_LIBRARY_DIR 
# GTKPLUS_LINK_FLAGS
# GTKPLUS_C_FLAGS
# GTKPLUS_INCLUDE_DIR


# Copyright: (C) 2009 RobotCub Consortium
# Authors: Alessandro Scalzo, Lorenzo Natale, Paul Fitzpatrick, Gianluca Massera
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Tried to make variables uniform with cmake "standards"
# Now set GtkPlus_* variables (respect capitalization rule).
# Maintain backwards compatibiliy.
# Lorenzo Natale 14/05/2010


# first check if we are using msvc
if(WIN32)
    find_package(GtkWin32)

    set(GtkPlus_LIBRARIES "${GTKPLUS_LINK_FLAGS}" CACHE STRING "GTK+ link flags")
    set(GtkPlus_INCLUDE_DIRS "${GTKPLUS_INCLUDE_DIR}" CACHE STRING "GTK+ include directory")

    if(GTK_FOUND)
        set(GtkPlus_FOUND TRUE)
        set(GtkPlus_LIBRARIES "${GTK_LIBRARIES}" CACHE STRING "GTK+ link flags")
        set(GtkPlus_INCLUDE_DIRS "${GTK_INCLUDE_DIR}" CACHE STRING "GTK+ include directory")
    endif(GTK_FOUND)
endif()

if(UNIX)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(GTKPLUS "gtk+-2.0")
        if(GTKPLUS_FOUND)
            set(GTKPLUS_INCLUDE_DIR ${GTKPLUS_INCLUDE_DIRS})
            set(GTKPLUS_LIBRARY_DIR ${GTKPLUS_LIBRARY_DIRS})
            set(GTKPLUS_LINK_FLAGS ${GTKPLUS_LDFLAGS})
            set(GTKPLUS_C_FLAGS ${GTKPLUS_CFLAGS})
        endif(GTKPLUS_FOUND)
    endif(PKG_CONFIG_FOUND)

    if(APPLE)
        # Make the following hardcoded links conditional, since e.g. a "brew"
        # installed version of gtk+ will already have been found.
        if(NOT GTKPLUS_LIBRARY_DIR)
            if(PKG_CONFIG_FOUND)
                set(ENV{PKG_CONFIG_PATH} $ENV{PKG_CONFIG_PATH}:/usr/X11/lib/pkgconfig)
                pkg_check_modules(GTKPLUS "gtk+-2.0")
                if(GTKPLUS_FOUND)
                    set(GTKPLUS_INCLUDE_DIR ${GTKPLUS_INCLUDE_DIRS})
                    set(GTKPLUS_LIBRARY_DIR ${GTKPLUS_LIBRARY_DIRS})
                    set(GTKPLUS_LINK_FLAGS ${GTKPLUS_LDFLAGS})
                    set(GTKPLUS_C_FLAGS ${GTKPLUS_CFLAGS})
                endif(GTKPLUS_FOUND)
            else(PKG_CONFIG_FOUND)
                ### THERE IS AN OFFICIAL GTK PACKAGE on www.gtk-osx.org
                ### The following setup is relative to the official GTK package for mac os-x
                set( GTKPLUS_INCLUDE_DIR "/Library/Frameworks/Gtk.framework/Headers;/Library/Frameworks/GLib.framework/Headers;/Library/Frameworks/Cairo.framework/Headers;")
                set( GTKPLUS_LIBRARY_DIR " " )
                set( GTKPLUS_LINK_FLAGS "-framework Gtk -framework GLib" )
                set( GTKPLUS_C_FLAGS " " )
            endif(PKG_CONFIG_FOUND)
        endif(NOT GTKPLUS_LIBRARY_DIR)
    endif(APPLE)

    set(GTKPLUS_LINK_FLAGS "${GTKPLUS_LINK_FLAGS}" CACHE INTERNAL "GTK+ link flags")
    set(GTKPLUS_C_FLAGS "${GTKPLUS_C_FLAGS}" CACHE INTERNAL "GTK+ include flags")
    set(GTKPLUS_INCLUDE_DIR "${GTKPLUS_INCLUDE_DIR}" CACHE INTERNAL "GTK+ include directory")
    set(GTKPLUS_LIBRARY_DIR "${GTKPLUS_LIBRARY_DIR}" CACHE INTERNAL "GTK+ library directory")

    #message(STATUS "Libraries --> ${GtkPlus_LIBRARIES}")
    #message(STATUS "IncludeDirs --> ${GtkPlus_INCLUDE_DIRS}")
    #message(STATUS "LinkFlags --> ${GTKPLUS_LINK_DIR}")
    #message(STATUS "CFlags --> ${GTKPLUS_C_FLAGS}")

    set(GtkPlus_LIBRARIES "${GTKPLUS_LINK_FLAGS}" CACHE STRING "GTK+ link flags")
    set(GtkPlus_INCLUDE_DIRS "${GTKPLUS_INCLUDE_DIR}" CACHE STRING "GTK+ include directory")

    if(GTKPLUS_C_FLAGS)
        set(GtkPlus_FOUND TRUE)
    else(GTKPLUS_C_FLAGS)
        set(GtkPlus_FOUND FALSE)
    endif(GTKPLUS_C_FLAGS)
endif(UNIX)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GtkPlus DEFAULT_MSG GtkPlus_LIBRARIES GtkPlus_INCLUDE_DIRS)

