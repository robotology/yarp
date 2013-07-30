# Copyright (C) 2013  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


# GooCanvasMM
project(YARP_goocanvasmm)

set(GooCanvasMM_GTK2_INCLUDE_DIRS ${GTK2_ATKMM_INCLUDE_DIR}
                                  ${GTK2_CAIROMMCONFIG_INCLUDE_DIR}
                                  ${GTK2_CAIROMM_INCLUDE_DIR}
                                  ${GTK2_GDKMMCONFIG_INCLUDE_DIR}
                                  ${GTK2_GDKMM_INCLUDE_DIR}
                                  ${GTK2_GIOMM_INCLUDE_DIR}
                                  ${GTK2_GLIBMMCONFIG_INCLUDE_DIR}
                                  ${GTK2_GLIBMM_INCLUDE_DIR}
                                  ${GTK2_GTKMMCONFIG_INCLUDE_DIR}
                                  ${GTK2_GTKMM_INCLUDE_DIR}
                                  ${GTK2_PANGOMMCONFIG_INCLUDE_DIR}
                                  ${GTK2_PANGOMM_INCLUDE_DIR}
                                  ${GTK2_SIGC++CONFIG_INCLUDE_DIR}
                                  ${GTK2_SIGC++_INCLUDE_DIR})
if(GTK2_GIOMMCONFIG_INCLUDE_DIR)
    list(APPEND GooCanvasMM_GTK2_INCLUDE_DIRS ${GTK2_GIOMMCONFIG_INCLUDE_DIR})
endif()
set(GooCanvasMM_GTK2_LIBRARIES ${GTK2_ATKMM_LIBRARY}
                               ${GTK2_CAIROMM_LIBRARY}
                               ${GTK2_GDKMM_LIBRARY}
                               ${GTK2_GLIBMM_LIBRARY}
                               ${GTK2_GTKMM_LIBRARY}
                               ${GTK2_PANGOMM_LIBRARY}
                               ${GTK2_SIGC++_LIBRARY})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/goocanvasmm/goocanvas/
                    ${GooCanvas_INCLUDE_DIRS}
                    ${GooCanvasMM_GTK2_INCLUDE_DIRS})


set(goocanvasmm_SRCS
    goocanvasmm/goocanvas/goocanvasmm/itemmodel.cc
    goocanvasmm/goocanvas/goocanvasmm/tablemodel.cc
    goocanvasmm/goocanvas/goocanvasmm/points.cc
    goocanvasmm/goocanvas/goocanvasmm/style.cc
    goocanvasmm/goocanvas/goocanvasmm/polylinemodel.cc
    goocanvasmm/goocanvas/goocanvasmm/itemmodelsimple.cc
    goocanvasmm/goocanvas/goocanvasmm/text.cc
    goocanvasmm/goocanvas/goocanvasmm/rect.cc
    goocanvasmm/goocanvas/goocanvasmm/canvas.cc
    goocanvasmm/goocanvas/goocanvasmm/grid.cc
    goocanvasmm/goocanvas/goocanvasmm/polyline.cc
    goocanvasmm/goocanvas/goocanvasmm/item.cc
    goocanvasmm/goocanvas/goocanvasmm/pathmodel.cc
    goocanvasmm/goocanvas/goocanvasmm/wrap_init.cc
    goocanvasmm/goocanvas/goocanvasmm/linedash.cc
    goocanvasmm/goocanvas/goocanvasmm/gridmodel.cc
    goocanvasmm/goocanvas/goocanvasmm/group.cc
    goocanvasmm/goocanvas/goocanvasmm/path.cc
    goocanvasmm/goocanvas/goocanvasmm/imagemodel.cc
    goocanvasmm/goocanvas/goocanvasmm/table.cc
    goocanvasmm/goocanvas/goocanvasmm/groupmodel.cc
    goocanvasmm/goocanvas/goocanvasmm/ellipse.cc
    goocanvasmm/goocanvas/goocanvasmm/rectmodel.cc
    goocanvasmm/goocanvas/goocanvasmm/bounds.cc
    goocanvasmm/goocanvas/goocanvasmm/init.cc
    goocanvasmm/goocanvas/goocanvasmm/widget.cc
    goocanvasmm/goocanvas/goocanvasmm/textmodel.cc
    goocanvasmm/goocanvas/goocanvasmm/ellipsemodel.cc
    goocanvasmm/goocanvas/goocanvasmm/enums.cc
    goocanvasmm/goocanvas/goocanvasmm/image.cc
    goocanvasmm/goocanvas/goocanvasmm/itemsimple.cc
    goocanvasmm/codegen/extradefs/generate_extra_defs_goocanvas.cc
)

set(goocanvasmm_HDRS
    goocanvasmm/goocanvas/goocanvasmm.h
    goocanvasmm/goocanvas/goocanvasmm/rectmodel.h
    goocanvasmm/goocanvas/goocanvasmm/polylinemodel.h
    goocanvasmm/goocanvas/goocanvasmm/itemmodelsimple.h
    goocanvasmm/goocanvas/goocanvasmm/wrap_init.h
    goocanvasmm/goocanvas/goocanvasmm/itemmodel.h
    goocanvasmm/goocanvas/goocanvasmm/linedash.h
    goocanvasmm/goocanvas/goocanvasmm/bounds.h
    goocanvasmm/goocanvas/goocanvasmm/table.h
    goocanvasmm/goocanvas/goocanvasmm/pathmodel.h
    goocanvasmm/goocanvas/goocanvasmm/init.h
    goocanvasmm/goocanvas/goocanvasmm/imagemodel.h
    goocanvasmm/goocanvas/goocanvasmm/textmodel.h
    goocanvasmm/goocanvas/goocanvasmm/item.h
    goocanvasmm/goocanvas/goocanvasmm/grid.h
    goocanvasmm/goocanvas/goocanvasmm/points.h
    goocanvasmm/goocanvas/goocanvasmm/private/polylinemodel_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/rect_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/item_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/points_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/polyline_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/groupmodel_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/imagemodel_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/linedash_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/rectmodel_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/image_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/ellipsemodel_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/enums_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/canvas_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/style_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/textmodel_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/gridmodel_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/ellipse_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/itemmodel_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/widget_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/itemsimple_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/table_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/grid_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/group_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/text_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/itemmodelsimple_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/path_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/tablemodel_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/pathmodel_p.h
    goocanvasmm/goocanvas/goocanvasmm/private/bounds_p.h
    goocanvasmm/goocanvas/goocanvasmm/canvas.h
    goocanvasmm/goocanvas/goocanvasmm/types.h
    goocanvasmm/goocanvas/goocanvasmm/style.h
    goocanvasmm/goocanvas/goocanvasmm/group.h
    goocanvasmm/goocanvas/goocanvasmm/tablemodel.h
    goocanvasmm/goocanvas/goocanvasmm/itemsimple.h
    goocanvasmm/goocanvas/goocanvasmm/widget.h
    goocanvasmm/goocanvas/goocanvasmm/gridmodel.h
    goocanvasmm/goocanvas/goocanvasmm/text.h
    goocanvasmm/goocanvas/goocanvasmm/groupmodel.h
    goocanvasmm/goocanvas/goocanvasmm/image.h
    goocanvasmm/goocanvas/goocanvasmm/polyline.h
    goocanvasmm/goocanvas/goocanvasmm/enums.h
    goocanvasmm/goocanvas/goocanvasmm/path.h
    goocanvasmm/goocanvas/goocanvasmm/ellipsemodel.h
    goocanvasmm/goocanvas/goocanvasmm/rect.h
    goocanvasmm/goocanvas/goocanvasmm/ellipse.h
)

add_library(YARP_goocanvasmm STATIC ${goocanvasmm_SRCS} ${goocanvasmm_HDRS})

target_link_libraries(YARP_goocanvasmm LINK_PRIVATE ${GooCanvas_LIBRARIES}
                                                    ${GooCanvasMM_GTK2_LIBRARIES})

if(MSVC)
    add_definitions(/wd4099 /wd4250 /wd4520 /wd4805 /vd2)
endif(MSVC)

if(UNIX)
    set_target_properties(YARP_goocanvasmm PROPERTIES COMPILE_FLAGS -fPIC)
    if (GtkMM_VERSION_MAJOR GREATER 2 OR GtkMM_VERSION_MAJOR EQUAL 2)
        if (GtkMM_VERSION_MINOR LESS 22)
            add_definitions(-DGTKMM_LESS_2_22)
         endif()
    endif()
endif(UNIX)

set(GooCanvasMM_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/goocanvasmm/goocanvas
                             ${GooCanvas_INCLUDE_DIRS}
                             ${GooCanvasMM_GTK2_INCLUDE_DIRS}
                             CACHE PATH "GooCanvasMM include directory" FORCE)
set(GooCanvasMM_LIBRARIES YARP_goocanvasmm CACHE STRING "GooCanvasMM libraries" FORCE)
set(GooCanvasMM_DEFINITIONS "" CACHE STRING "Additional compiler flags for GooCanvasMM" FORCE)

mark_as_advanced(GooCanvasMM_INCLUDE_DIRS GooCanvasMM_LIBRARIES GooCanvasMM_DEFINITIONS)
