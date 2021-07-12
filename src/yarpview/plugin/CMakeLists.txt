# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

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

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${QtYARPViewPlugin_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${QtYARPViewPlugin_HDRS})
source_group("Resources Files" FILES ${QtYARPViewPlugin_QRC_FILES})
source_group("QML Files" FILES ${QtYARPViewPlugin_QML_FILES})
source_group("Generated Files" FILES ${QtYARPViewPlugin_QRC_GEN_SRCS})

add_library(QtYARPViewPlugin MODULE)

target_sources(QtYARPViewPlugin PRIVATE ${QtYARPViewPlugin_SRCS}
                                        ${QtYARPViewPlugin_HDRS}
                                        ${QtYARPViewPlugin_QRC_FILES}
                                        ${QtYARPViewPlugin_QRC_GEN_SRCS}
                                        ${QtYARPViewPlugin_QML_FILES})

target_link_libraries(QtYARPViewPlugin PRIVATE YARP::YARP_os
                                               YARP::YARP_init
                                               YARP::YARP_sig
                                               Qt5::Widgets
                                               Qt5::Qml
                                               Qt5::Quick
                                               Qt5::Multimedia)

qtyarp_qml_plugin(QtYARPViewPlugin "${QtYARPViewPlugin_QMLDIR}")

install(TARGETS QtYARPViewPlugin
        COMPONENT utilities
        RUNTIME DESTINATION "${CMAKE_INSTALL_QMLDIR}/${QtYARPViewPlugin_QMLDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_QMLDIR}/${QtYARPViewPlugin_QMLDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_QMLDIR}/${QtYARPViewPlugin_QMLDIR}")

install(FILES qmldir DESTINATION "${CMAKE_INSTALL_QMLDIR}/${QtYARPViewPlugin_QMLDIR}")

set_property(TARGET QtYARPViewPlugin PROPERTY FOLDER "Plugins/QML")
