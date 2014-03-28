# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


# GtkDatabox
project(YARP_gtkdatabox)

set(GtkDatabox_GTK2_INCLUDE_DIRS ${GTK2_ATK_INCLUDE_DIR}
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
set(GtkDatabox_GTK2_LIBRARIES ${GTK2_CAIRO_LIBRARY}
                              ${GTK2_GDK_LIBRARY}
                              ${GTK2_GLIB_LIBRARY}
                              ${GTK2_GOBJECT_LIBRARY}
                              ${GTK2_GTK_LIBRARY}
                              ${GTK2_PANGO_LIBRARY})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gtkdatabox/gtk
                    ${GtkDatabox_GTK2_INCLUDE_DIRS})


set(gtkdatabox_SRCS
    gtkdatabox/gtk/gtkdatabox.c
    gtkdatabox/gtk/gtkdatabox_bars.c
    gtkdatabox/gtk/gtkdatabox_cross_simple.c
    gtkdatabox/gtk/gtkdatabox_graph.c
    gtkdatabox/gtk/gtkdatabox_grid.c
    gtkdatabox/gtk/gtkdatabox_lines.c
    gtkdatabox/gtk/gtkdatabox_markers.c
    gtkdatabox/gtk/gtkdatabox_marshal.c
    gtkdatabox/gtk/gtkdatabox_offset_bars.c
    gtkdatabox/gtk/gtkdatabox_points.c
    gtkdatabox/gtk/gtkdatabox_regions.c
    gtkdatabox/gtk/gtkdatabox_ruler.c
    gtkdatabox/gtk/gtkdatabox_scale.c
    gtkdatabox/gtk/gtkdatabox_xyc_graph.c
    gtkdatabox/gtk/gtkdatabox_xyyc_graph.c
)

set(gtkdatabox_HDRS
    gtkdatabox/gtk/gtkdatabox.h
    gtkdatabox/gtk/gtkdatabox_bars.h
    gtkdatabox/gtk/gtkdatabox_cross_simple.h
    gtkdatabox/gtk/gtkdatabox_graph.h
    gtkdatabox/gtk/gtkdatabox_grid.h
    gtkdatabox/gtk/gtkdatabox_lines.h
    gtkdatabox/gtk/gtkdatabox_markers.h
    gtkdatabox/gtk/gtkdatabox_marshal.h
    gtkdatabox/gtk/gtkdatabox_offset_bars.h
    gtkdatabox/gtk/gtkdatabox_points.h
    gtkdatabox/gtk/gtkdatabox_regions.h
    gtkdatabox/gtk/gtkdatabox_ruler.h
    gtkdatabox/gtk/gtkdatabox_scale.h
    gtkdatabox/gtk/gtkdatabox_typedefs.h
    gtkdatabox/gtk/gtkdatabox_xyc_graph.h
    gtkdatabox/gtk/gtkdatabox_xyyc_graph.h
)

add_library(YARP_gtkdatabox STATIC ${gtkdatabox_SRCS})

target_link_libraries(YARP_gtkdatabox LINK_PRIVATE ${GtkDatabox_GTK2_LIBRARIES})

if(MSVC)
    add_definitions(/wd4305 /wd4244)
endif(MSVC)

if(UNIX)
    set_target_properties(YARP_gtkdatabox PROPERTIES COMPILE_FLAGS "-fPIC")

#    include(CheckCCompilerFlag)
#    check_c_compiler_flag("-Wno-unused-but-set-variable" C_SUPPORTS_WNO_UNUSED_BUT_SET_VARIABLE)
#    if(C_SUPPORTS_WNO_UNUSED_BUT_SET_VARIABLE)
#        set_source_files_properties(gtkdatabox/gtk/gtkdatabox_bars.c
#                                    gtkdatabox/gtk/gtkdatabox_lines.c
#                                    gtkdatabox/gtk/gtkdatabox_ruler.c
#                                    PROPERTIES COMPILE_FLAGS "-Wno-unused-but-set-variable")
#    else(C_SUPPORTS_WNO_UNUSED_BUT_SET_VARIABLE)
#        set_source_files_properties(gtkdatabox/gtk/gtkdatabox_bars.c
#                                    gtkdatabox/gtk/gtkdatabox_lines.c
#                                    gtkdatabox/gtk/gtkdatabox_ruler.c
#                                    PROPERTIES COMPILE_FLAGS "-Wno-unused-variable")
#    endif(C_SUPPORTS_WNO_UNUSED_BUT_SET_VARIABLE)
endif(UNIX)

set(GtkDatabox_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/gtkdatabox/gtk
                            ${GtkDatabox_GTK2_INCLUDE_DIRS}
                            PARENT_SCOPE)
set(GtkDatabox_LIBRARIES YARP_gtkdatabox PARENT_SCOPE)
set(GtkDatabox_DEFINITIONS "" PARENT_SCOPE)
