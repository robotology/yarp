set(YarpScope_SRCS
    main.cpp
    DataPlot.cpp
    Debug.cpp
    Graph.cpp
    MainWindow.cpp
    PortReader.cpp
    PlotManager.cpp
    XmlLoader.cpp
)


# TinyXML
set(tinyxml_SRCS
    tinyxml/tinystr.cpp
    tinyxml/tinyxml.cpp
    tinyxml/tinyxmlerror.cpp
    tinyxml/tinyxmlparser.cpp
)

set(YarpScope_SRCS
    ${YarpScope_SRCS}
    ${tinyxml_SRCS}
)


# GtkDatabox and GtkDataboxMM
set(gtkdatabox_SRCS
    gtkdatabox/gtkdatabox_bars.c
    gtkdatabox/gtkdatabox.c
    gtkdatabox/gtkdatabox_cross_simple.c
    gtkdatabox/gtkdatabox_graph.c
    gtkdatabox/gtkdatabox_grid.c
    gtkdatabox/gtkdatabox_lines.c
    gtkdatabox/gtkdatabox_markers.c
    gtkdatabox/gtkdatabox_marshal.c
    gtkdatabox/gtkdatabox_offset_bars.c
    gtkdatabox/gtkdatabox_points.c
    gtkdatabox/gtkdatabox_regions.c
    gtkdatabox/gtkdatabox_ruler.c
    gtkdatabox/gtkdatabox_scale.c
    gtkdatabox/gtkdatabox_xyc_graph.c
    gtkdatabox/gtkdatabox_xyyc_graph.c
)

set(gtkdataboxmm_SRCS
    gtkdatabox/gtkdataboxmm/bars.cc
    gtkdatabox/gtkdataboxmm/cross_simple.cc
    gtkdatabox/gtkdataboxmm/databox.cc
    gtkdatabox/gtkdataboxmm/graph.cc
    gtkdatabox/gtkdataboxmm/grid.cc
    gtkdatabox/gtkdataboxmm/init.cc
    gtkdatabox/gtkdataboxmm/lines.cc
    gtkdatabox/gtkdataboxmm/markers.cc
    gtkdatabox/gtkdataboxmm/points.cc
    gtkdatabox/gtkdataboxmm/ruler.cc
    gtkdatabox/gtkdataboxmm/scale.cc
    gtkdatabox/gtkdataboxmm/wrap_init.cc
    gtkdatabox/gtkdataboxmm/xyc_graph.cc
)

if(NOT YARPSCOPE_USE_SYSTEM_GTKDATABOXMM)
    include_directories(gtkdatabox)
    set(YarpScope_SRCS
        ${YarpScope_SRCS}
        ${gtkdataboxmm_SRCS}
    )
    if(NOT YARPSCOPE_USE_SYSTEM_GTKDATABOX)
        include_directories(gtkdatabox)
        set(YarpScope_SRCS
            ${YarpScope_SRCS}
            ${gtkdatabox_SRCS}
        )
    endif(NOT YARPSCOPE_USE_SYSTEM_GTKDATABOX)
endif(NOT YARPSCOPE_USE_SYSTEM_GTKDATABOXMM)



add_executable(yarpscope ${YarpScope_SRCS})


target_link_libraries(yarpscope
                      ${YARP_LIBRARIES}
                      ${GTKMM_LIBRARIES}
)

if(YARPSCOPE_USE_SYSTEM_GTKDATABOXMM)
    target_link_libraries(yarpscope ${GTKDATABOXMM_LIBRARIES})
else(YARPSCOPE_USE_SYSTEM_GTKDATABOXMM)
    if(YARPSCOPE_USE_SYSTEM_GTKDATABOX)
        target_link_libraries(yarpscope ${GTKDATABOX_LIBRARIES})
    endif(YARPSCOPE_USE_SYSTEM_GTKDATABOX)
endif(YARPSCOPE_USE_SYSTEM_GTKDATABOXMM)

install(TARGETS yarpscope COMPONENT utilities DESTINATION bin)
