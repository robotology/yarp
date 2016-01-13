# Copyright (C) 2013  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


# GooCanvas
project(YARP_priv_goocanvas)

set(GooCanvas_GTK2_INCLUDE_DIRS ${GTK2_ATK_INCLUDE_DIR}
                                ${GTK2_CAIRO_INCLUDE_DIR}
                                ${GTK2_FONTCONFIG_INCLUDE_DIR}
                                ${GTK2_GDKCONFIG_INCLUDE_DIR}
                                ${GTK2_GDK_INCLUDE_DIR}
                                ${GTK2_GDK_PIXBUF_INCLUDE_DIR}
                                ${GTK2_GLIBCONFIG_INCLUDE_DIR}
                                ${GTK2_GLIB_INCLUDE_DIR}
                                ${GTK2_GOBJECT_INCLUDE_DIR}
                                ${GTK2_GTK_INCLUDE_DIR}
                                ${GTK2_PANGO_INCLUDE_DIR}
                                ${FREETYPE_INCLUDE_DIR_freetype2}
                                ${FREETYPE_INCLUDE_DIR_ft2build})
set(GooCanvas_GTK2_LIBRARIES ${GTK2_ATK_LIBRARY}
                             ${GTK2_CAIRO_LIBRARY}
                             ${GTK2_GDK_LIBRARY}
                             ${GTK2_GDK_PIXBUF_LIBRARY}
                             ${GTK2_GLIB_LIBRARY}
                             ${GTK2_GOBJECT_LIBRARY}
                             ${GTK2_GTK_LIBRARY}
                             ${GTK2_PANGO_LIBRARY}
                             ${GTK2_PANGOCAIRO_LIBRARY})



include_directories(${CMAKE_CURRENT_SOURCE_DIR}/goocanvas
                    ${CMAKE_CURRENT_SOURCE_DIR}/goocanvas/src)
include_directories(SYSTEM ${GooCanvas_GTK2_INCLUDE_DIRS})


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
    goocanvas/src/goocanvaspolyline.c)

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
    goocanvas/src/goocanvasrect.h)

add_definitions(${GTK2_DEFINITIONS})

if(MSVC)
    add_definitions(/D_USE_MATH_DEFINES /wd4018 /wd4244)
endif(MSVC)

add_library(YARP_priv_goocanvas STATIC ${goocanvas_SRCS} ${goocanvas_HDRS})

target_link_libraries(YARP_priv_goocanvas LINK_PRIVATE ${GooCanvas_GTK2_LIBRARIES})

set_property(TARGET YARP_priv_goocanvas PROPERTY FOLDER "External Libraries")

if(UNIX)
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

set(GooCanvas_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/goocanvas/src
                           ${GooCanvas_GTK2_INCLUDE_DIRS}
                           PARENT_SCOPE)
set(GooCanvas_LIBRARIES YARP_priv_goocanvas PARENT_SCOPE)
set(GooCanvas_DEFINITIONS "" PARENT_SCOPE)
