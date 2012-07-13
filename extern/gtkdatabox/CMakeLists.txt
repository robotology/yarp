# GtkDatabox
project(gtkdatabox)

include_directories(${GTKPLUS_INCLUDE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/gtkdatabox/gtk
)

set(gtkdatabox_SRCS
    gtkdatabox/gtk/gtkdatabox.c
    gtkdatabox/gtk/gtkdatabox_bars.c
    gtkdatabox/gtk/gtkdatabox_cross_simple.c
    gtkdatabox/gtk/gtkdatabox_graph.c
    gtkdatabox/gtk/gtkdatabox_grid.c
    gtkdatabox/gtk/gtkdatabox_lines.c
    gtkdatabox/gtk/gtkdatabox_markers.c
    gtkdatabox/gtk/gtkdatabox_marshal.c
    gtkdatabox/gtk/gtkdatabox_points.c
    gtkdatabox/gtk/gtkdatabox_ruler.c
    gtkdatabox/gtk/gtkdatabox_scale.c
    gtkdatabox/gtk/gtkdatabox_xyc_graph.c
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
    gtkdatabox/gtk/gtkdatabox_points.h
    gtkdatabox/gtk/gtkdatabox_ruler.h
    gtkdatabox/gtk/gtkdatabox_scale.h
    gtkdatabox/gtk/gtkdatabox_typedefs.h
    gtkdatabox/gtk/gtkdatabox_xyc_graph.h
)

add_library(gtkdatabox STATIC ${gtkdatabox_SRCS})

set(GTKDATABOX_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/gtkdatabox/gtk" CACHE PATH "GtkDatabox include directory" FORCE)
if(YARP_USE_GTK2)
    set(GTKDATABOX_LIBRARY_DIRS "${CMAKE_BINARY_DIR}/lib;${GTK2_LIBRARY_DIRS}" CACHE PATH "GtkDatabox library directory" FORCE)
    set(GTKDATABOX_LIBRARIES "gtkdatabox;${GTK2_LIBRARIES}" CACHE STRING "GtkDatabox libraries" FORCE)
else(YARP_USE_GTK2)
    set(GTKDATABOX_LIBRARY_DIRS "${CMAKE_BINARY_DIR}/lib;${GTKPLUS_LIBRARY_DIRS}" CACHE PATH "GtkDatabox library directory" FORCE)
    set(GTKDATABOX_LIBRARIES "gtkdatabox;${GTKPLUS_LIBRARIES}" CACHE STRING "GtkDatabox libraries" FORCE)
endif(YARP_USE_GTK2)
mark_as_advanced(GTKDATABOX_INCLUDE_DIRS GTKDATABOX_LIBRARY_DIRS GTKDATABOX_LIBRARIES)
