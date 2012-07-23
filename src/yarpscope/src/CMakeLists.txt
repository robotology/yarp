# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


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
                      ${TINYXML_LIBRARIES}
                      ${GTKDATABOXMM_LIBRARIES}
)

install(TARGETS yarpscope COMPONENT utilities DESTINATION bin)
