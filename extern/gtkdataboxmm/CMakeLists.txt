# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


# GtkDataboxMM
project(YARP_gtkdataboxmm)

if(YARP_USE_GTK2)
    set(GtkDataboxMM_GTK2_INCLUDE_DIRS ${GTK2_ATKMM_INCLUDE_DIR}
                                       ${GTK2_CAIROMMCONFIG_INCLUDE_DIR}
                                       ${GTK2_CAIROMM_INCLUDE_DIR}
                                       ${GTK2_GDKMMCONFIG_INCLUDE_DIR}
                                       ${GTK2_GDKMM_INCLUDE_DIR}
                                       ${GTK2_GIOMMCONFIG_INCLUDE_DIR}
                                       ${GTK2_GIOMM_INCLUDE_DIR}
                                       ${GTK2_GLIBMMCONFIG_INCLUDE_DIR}
                                       ${GTK2_GLIBMM_INCLUDE_DIR}
                                       ${GTK2_GTKMMCONFIG_INCLUDE_DIR}
                                       ${GTK2_GTKMM_INCLUDE_DIR}
                                       ${GTK2_PANGOMMCONFIG_INCLUDE_DIR}
                                       ${GTK2_PANGOMM_INCLUDE_DIR}
                                       ${GTK2_SIGC++CONFIG_INCLUDE_DIR}
                                       ${GTK2_SIGC++_INCLUDE_DIR})
    set(GtkDataboxMM_GTK2_LIBRARIES ${GTK2_ATKMM_LIBRARY}
                                    ${GTK2_GDKMM_LIBRARY}
                                    ${GTK2_GLIBMM_LIBRARY}
                                    ${GTK2_GTKMM_LIBRARY}
                                    ${GTK2_SIGC++_LIBRARY})
else(YARP_USE_GTK2)
    set(GtkDataboxMM_GTK2_INCLUDE_DIRS ${GTKMM_INCLUDE_DIRS})
    set(GtkDataboxMM_GTK2_LIBRARIES ${GTKMM_LIBRARIES})
endif(YARP_USE_GTK2)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gtkdataboxmm/gtkdatabox/
                    ${GtkDatabox_INCLUDE_DIRS}
                    ${GtkDataboxMM_GTK2_INCLUDE_DIRS})


set(gtkdataboxmm_SRCS
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/points.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/databox.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/graph.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/cross_simple.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/markers.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/ruler.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/grid.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/xyc_graph.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/wrap_init.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/scale.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/bars.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/init.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/lines.cc
)

set(gtkdataboxmm_HDRS
    gtkdataboxmm/gtkdatabox/gtkdataboxmmconfig.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/wrap_init.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/cross_simple.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/xyc_graph.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/init.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/bars.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/scale.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/grid.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/points.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/markers.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/lines.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/cross_simple_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/lines_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/points_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/bars_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/markers_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/ruler_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/databox_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/xyc_graph_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/graph_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/grid_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/scale_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/databox.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/graph.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/ruler.h
)

add_library(YARP_gtkdataboxmm STATIC ${gtkdataboxmm_SRCS})

target_link_libraries(YARP_gtkdataboxmm LINK_PRIVATE ${GtkDatabox_LIBRARIES}
                                                     ${GtkDataboxMM_GTK2_LIBRARIES})

if(MSVC)
    add_definitions(/wd4250)
endif(MSVC)

if(UNIX)
    set_target_properties(YARP_gtkdataboxmm PROPERTIES COMPILE_FLAGS -fPIC)
endif(UNIX)

set(GtkDataboxMM_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/gtkdataboxmm/gtkdatabox
                              ${GtkDatabox_INCLUDE_DIRS}
                              ${GtkDataboxMM_GTK2_INCLUDE_DIRS}
                              CACHE PATH "GtkDataboxMM include directory" FORCE)
set(GtkDataboxMM_LIBRARIES YARP_gtkdataboxmm CACHE STRING "GtkDataboxMM libraries" FORCE)
set(GtkDataboxMM_DEFINITIONS "" CACHE STRING "Additional compiler flags for GtkDataboxMM" FORCE)

mark_as_advanced(GtkDataboxMM_INCLUDE_DIRS GtkDataboxMM_LIBRARIES GtkDataboxMM_DEFINITIONS)
