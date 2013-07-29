# Try to find Gthread package
#
# On exit set:
# Gthread_FOUND
# Gthread_LIBRARIES
# Gthread_INCLUDE_DIRS
#
# Old variables:
#
# GTHREAD_FOUND
# GTHREAD_LINK_FLAGS
# GTHREAD_INCLUDE_DIR


# Copyright: (C) 2009 RobotCub Consortium
# Authors: Alessandro Scalzo, Lorenzo Natale, Paul Fitzpatrick, Gianluca Massera
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Tried to make variables uniform with cmake "standards"
# Now set Gthread_* variables (respect capitalization rule).
# Maintain backwards compatibiliy.
# Lorenzo Natale 14/05/2010

# Added: 03/11/2011, Lorenzo
# Windows: support gtkmm x64, search also using GTKMM64_BASEPATH (this variable is set by the installer).


if (NOT Gthread_FOUND)

    if(UNIX)

        find_package(PkgConfig QUIET)
        if (PKG_CONFIG_FOUND)
        pkg_check_modules(GTHREAD "gthread-2.0")
        if (GTHREAD_FOUND)
            set(GTHREAD_INCLUDE_DIR ${GTHREAD_INCLUDE_DIRS})
            set(GTHREAD_LIBRARY_DIR ${GTHREAD_LIBRARY_DIRS})
            set(GTHREAD_LINK_FLAGS ${GTHREAD_LDFLAGS})
            set(GTHREAD_C_FLAGS ${GTHREAD_CFLAGS})
        endif (GTHREAD_FOUND)
        endif (PKG_CONFIG_FOUND)

        if(APPLE)
            if(NOT GTHREAD_LINK_FLAGS)
                ### THERE IS AN OFFICIAL GTK PACKAGE on www.gtk-osx.org
                ### The following setup is relative to the official GTK package for mac os-x
                set( GTHREAD_INCLUDE_DIR "/Library/Frameworks/GLib.framework/Headers" )
                set( GTHREAD_LIBRARY_DIR " " )
                set( GTHREAD_LINK_FLAGS "-framework GLib" )
                set( GTHREAD_C_FLAGS " " )
            endif()
        endif(APPLE)

        set(GTHREAD_LINK_FLAGS "${GTHREAD_LINK_FLAGS}" CACHE INTERNAL "gthread link flags")
        set(GTHREAD_C_FLAGS "${GTHREAD_C_FLAGS}" CACHE INTERNAL "gthread include flags")
        set(GTHREAD_INCLUDE_DIR "${GTHREAD_INCLUDE_DIR}" CACHE INTERNAL "gthread include directory")

        set(Gthread_LIBRARIES "${GTHREAD_LINK_FLAGS}" CACHE INTERNAL "gthread link flags")
        set(Gthread_C_FLAGS "${GTHREAD_C_FLAGS}" CACHE INTERNAL "gthread include flags")
        set(Gthread_INCLUDE_DIRS "${GTHREAD_INCLUDE_DIR}" CACHE INTERNAL "gthread include directory")

        if(GTHREAD_C_FLAGS)
            set(Gthread_FOUND TRUE)
        else(GTHREAD_C_FLAGS)
            set(Gthread_FOUND FALSE)
        endif(GTHREAD_C_FLAGS)
        include(FindPackageHandleStandardArgs)

        find_package_handle_standard_args(Gthread DEFAULT_MSG Gthread_LIBRARIES Gthread_INCLUDE_DIRS)

    else(UNIX)

        # search GTKMM_BASEPATH to support gthread shipped and installed from gtkmm
        # priority to GTKMM64_BASEPATH (x64 build)
        set(GTK_BASEPATH $ENV{GTKMM64_BASEPATH})
        if (NOT GTK_BASEPATH)
            set(GTK_BASEPATH $ENV{GTKMM_BASEPATH})
        endif()

        if (NOT GTK_BASEPATH)
            set(GTK_BASEPATH $ENV{GTK_BASEPATH})
        endif()

        find_library(GTK_thread_lib
                     NAMES gthread-2.0
                     PATHS ${GTK_BASEPATH}/lib)

        set(Gthread_INCLUDE_DIRS "" CACHE PATH "Gthread include directory")

        if (GTK_thread_lib)
            set(Gthread_LIBRARIES ${GTK_thread_lib} CACHE STRING "Gthread libraries")
        endif(GTK_thread_lib)

        find_package_handle_standard_args(Gthread DEFAULT_MSG Gthread_LIBRARIES)

        set(Gthread_FOUND ${GTHREAD_FOUND})

    endif(UNIX)

    mark_as_advanced(Gthread_INCLUDE_DIRS Gthread_LIBRARIES)

endif (NOT Gthread_FOUND)


# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(Gthread PROPERTIES DESCRIPTION "Glib thread library"
                                              URL "http://developer.gnome.org/glib/")
endif()
