# Copyright (C) 2013  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


# GooCanvas
project(YARP_goocanvas)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/goocanvas
                    ${CMAKE_CURRENT_SOURCE_DIR}/goocanvas/src)

if(YARP_USE_GTK2)
    include_directories(${GTK2_INCLUDE_DIRS})
else(YARP_USE_GTK2)
    include_directories(${GtkPlus_INCLUDE_DIRS})
endif(YARP_USE_GTK2)

set(goocanvas_SRCS
    goocanvas/src/goocanvas.c
    goocanvas/src/goocanvasgroup.c
    goocanvas/src/goocanvasenumtypes.c
    goocanvas/src/goocanvasitem.c
    goocanvas/src/goocanvasimage.c
    goocanvas/src/goocanvasutils.c
    goocanvas/src/goocanvastable.c
    goocanvas/src/goocanvasstyle.c
    goocanvas/src/goocanvasmarshal.c
    goocanvas/src/goocanvasatk.c
    goocanvas/src/goocanvasellipse.c
    goocanvas/src/goocanvasitemmodel.c
    goocanvas/src/goocanvaspath.c
    goocanvas/src/goocanvasitemsimple.c
    goocanvas/src/goocanvaswidget.c
    goocanvas/src/goocanvastext.c
    goocanvas/src/goocanvasgrid.c
    goocanvas/src/goocanvasrect.c
    goocanvas/src/goocanvaspolyline.c
)

set(goocanvas_HDRS
    goocanvas/src/goocanvasgroup.h
    goocanvas/src/goocanvastable.h
    goocanvas/src/goocanvasprivate.h
    goocanvas/src/goocanvasitemmodel.h
    goocanvas/src/goocanvasimage.h
    goocanvas/src/goocanvasgrid.h
    goocanvas/src/goocanvasutils.h
    goocanvas/src/goocanvasenumtypes.h
    goocanvas/src/goocanvaswidget.h
    goocanvas/src/goocanvasstyle.h
    goocanvas/src/goocanvastext.h
    goocanvas/src/goocanvasitemsimple.h
    goocanvas/src/goocanvaspath.h
    goocanvas/src/goocanvas.h
    goocanvas/src/goocanvasatk.h
    goocanvas/src/goocanvasellipse.h
    goocanvas/src/goocanvaspolyline.h
    goocanvas/src/goocanvasmarshal.h
    goocanvas/src/goocanvasitem.h
    goocanvas/src/goocanvasrect.h
)

add_library(YARP_goocanvas STATIC ${goocanvas_SRCS})

if(UNIX)
    set_target_properties(YARP_goocanvas PROPERTIES COMPILE_FLAGS "-fPIC")

    include(CheckCCompilerFlag)
    check_c_compiler_flag("-Wno-unused-but-set-variable" C_SUPPORTS_WNO_UNUSED_BUT_SET_VARIABLE)
    if(C_SUPPORTS_WNO_UNUSED_BUT_SET_VARIABLE)
        set_source_files_properties(goocanvas/src/goocanvasgroup.c
                                    PROPERTIES COMPILE_FLAGS "-Wno-unused-but-set-variable")
    else(C_SUPPORTS_WNO_UNUSED_BUT_SET_VARIABLE)
        set_source_files_properties(goocanvas/src/goocanvasgroup.c
                                    PROPERTIES COMPILE_FLAGS "-Wno-unused-variable")
    endif(C_SUPPORTS_WNO_UNUSED_BUT_SET_VARIABLE)
endif(UNIX)

set(GooCanvas_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/goocanvas/src" CACHE PATH "GooCanvas include directory" FORCE)

if(YARP_USE_GTK2)
    set(GooCanvas_LIBRARY_DIRS "${CMAKE_BINARY_DIR}/lib" CACHE PATH "GooCanvas library directory" FORCE)
    set(GooCanvas_LIBRARIES "YARP_goocanvas;${GTK2_GLIB_LIBRARY};${GTK2_GOBJECT_LIBRARY};${GTK2_GDK_LIBRARY};${GTK2_GTK_LIBRARY};${GTK2_PANGO_LIBRARY};${GTK2_CAIRO_LIBRARY};" CACHE STRING "GooCanvas libraries" FORCE)
else(YARP_USE_GTK2)
    set(GooCanvas_LIBRARY_DIRS "${CMAKE_BINARY_DIR}/lib;${GTKPLUS_LIBRARY_DIRS}" CACHE PATH "GooCanvas library directory" FORCE)
    set(GooCanvas_LIBRARIES "YARP_goocanvas;${GTKPLUS_LIBRARIES}" CACHE STRING "GooCanvas libraries" FORCE)
endif(YARP_USE_GTK2)

mark_as_advanced(GooCanvas_INCLUDE_DIRS GooCanvas_LIBRARY_DIRS GooCanvas_LIBRARIES)
