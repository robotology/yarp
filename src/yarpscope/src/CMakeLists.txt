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

include_directories(${TINYXML_INCLUDE_DIRS}
                    ${GTKDATABOX_INCLUDE_DIRS}
)

link_directories(${TINYXML_LIBRARY_DIRS}
                 ${GTKDATABOX_LIBRARY_DIRS}
)

# GtkDataboxMM
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
endif(NOT YARPSCOPE_USE_SYSTEM_GTKDATABOXMM)



add_executable(yarpscope ${YarpScope_SRCS})

target_link_libraries(yarpscope
                      YARP_init YARP_OS
                      ${GTKMM_LIBRARIES}
                      ${TINYXML_LIBRARIES}
                      ${GTKDATABOX_LIBRARIES}
)

if(YARPSCOPE_USE_SYSTEM_GTKDATABOXMM)
    target_link_libraries(yarpscope ${GTKDATABOXMM_LIBRARIES})
endif(YARPSCOPE_USE_SYSTEM_GTKDATABOXMM)

install(TARGETS yarpscope COMPONENT utilities DESTINATION bin)
