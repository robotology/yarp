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
                    ${GTKDATABOXMM_INCLUDE_DIRS}
)

link_directories(${TINYXML_LIBRARY_DIRS}
                 ${GTKDATABOXMM_LIBRARY_DIRS}
)

add_executable(yarpscope ${YarpScope_SRCS})

target_link_libraries(yarpscope
                      YARP_init YARP_OS
                      ${GTKMM_LIBRARIES}
                      ${TINYXML_LIBRARIES}
                      ${GTKDATABOXMM_LIBRARIES}
)

install(TARGETS yarpscope COMPONENT utilities DESTINATION bin)
