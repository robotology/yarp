# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
include_directories(${YARP_OS_INCLUDE_DIRS}
                    ${YARP_sig_INCLUDE_DIRS})

set(QtYARPViewPlugin_SRCS qtyarpview_plugin.cpp
                          videoproducer.cpp
                          signalhandler.cpp
                          qtyarpview.cpp
                          ImagePort.cpp)

set(QtYARPViewPlugin_HDRS qtyarpview.h
                          FpsStats.h
                          videoproducer.h
                          signalhandler.h
                          ImagePort.h
                          qtyarpview_plugin.h)
set(QtYARPViewPlugin_QRC_FILES res.qrc)
set(QtYARPViewPlugin_QML_FILES YARPViewMenu.qml
                               VideoSurface.qml
                               YARPViewStatusBar.qml)

qt5_add_resources(QtYARPViewPlugin_QRC_GEN_SRCS ${QtYARPViewPlugin_QRC_FILES})

source_group("Source Files" FILES ${QtYARPViewPlugin_SRCS})
source_group("Header Files" FILES ${QtYARPViewPlugin_HDRS})
source_group("Resources Files" FILES ${QtYARPViewPlugin_QRC_FILES})
source_group("QML Files" FILES ${QtYARPViewPlugin_QML_FILES})
source_group("Generated Files" FILES ${QtYARPViewPlugin_QRC_GEN_SRCS})

set(QtYARPViewPlugin_QMLDIR "robotology/yarp/view")

add_library(QtYARPViewPlugin MODULE ${QtYARPViewPlugin_SRCS}
                                    ${QtYARPViewPlugin_HDRS}
                                    ${QtYARPViewPlugin_QRC_GEN_SRCS} )
target_link_libraries(QtYARPViewPlugin YARP_OS
                                       YARP_init
                                       YARP_sig)
qt5_use_modules(QtYARPViewPlugin Widgets
                                 Qml
                                 Quick
                                 Multimedia)

qtyarp_qml_plugin(QtYARPViewPlugin "${QtYARPViewPlugin_QMLDIR}")

install(TARGETS QtYARPViewPlugin
        COMPONENT utilities
        RUNTIME DESTINATION "${CMAKE_INSTALL_QMLDIR}/${QtYARPViewPlugin_QMLDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_QMLDIR}/${QtYARPViewPlugin_QMLDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_QMLDIR}/${QtYARPViewPlugin_QMLDIR}")

install(FILES qmldir DESTINATION "${CMAKE_INSTALL_QMLDIR}/${QtYARPViewPlugin_QMLDIR}")
