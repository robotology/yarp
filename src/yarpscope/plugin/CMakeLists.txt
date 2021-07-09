# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include_directories(SYSTEM ${QCustomPlot_INCLUDE_DIRS})

set(QtYARPScopePlugin_SRCS portreader.cpp
                           simpleloader.cpp
                           qtyarpscope.cpp
                           plotter.cpp
                           genericloader.cpp
                           xmlloader.cpp
                           plotmanager.cpp
                           qtyarpscopeplugin_plugin.cpp)
set(QtYARPScopePlugin_HDRS portreader.h
                           qtyarpscope.h
                           genericloader.h
                           plotmanager.h
                           qtyarpscopeplugin_plugin.h
                           plotter.h
                           xmlloader.h
                           simpleloader.h)
set(QtYARPScopePlugin_QRC_FILES res.qrc)
set(QtYARPScopePlugin_QML_FILES YARPScopeAbout.qml
                                YARPScopeMenu.qml
                                YARPScopeToolBar.qml)

qt5_add_resources(QtYARPScopePlugin_QRC_GEN_SRCS ${QtYARPScopePlugin_QRC_FILES})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${QtYARPScopePlugin_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${QtYARPScopePlugin_HDRS})
source_group("Resources Files" FILES ${QtYARPScopePlugin_QRC_FILES})
source_group("QML Files" FILES ${QtYARPScopePlugin_QML_FILES})
source_group("Generated Files" FILES ${QtYARPScopePlugin_QRC_GEN_SRCS})

add_library(QtYARPScopePlugin MODULE)

target_sources(QtYARPScopePlugin PRIVATE ${QtYARPScopePlugin_SRCS}
                                         ${QtYARPScopePlugin_HDRS}
                                         ${QtYARPScopePlugin_QRC_FILES}
                                         ${QtYARPScopePlugin_QRC_GEN_SRCS}
                                         ${QtYARPScopePlugin_QML_FILES})

target_link_libraries(QtYARPScopePlugin PRIVATE YARP::YARP_os
                                                YARP::YARP_init
                                                ${QCustomPlot_LIBRARIES}
                                                Qt5::Widgets
                                                Qt5::Qml
                                                Qt5::Quick
                                                Qt5::Xml
                                                Qt5::PrintSupport)

qtyarp_qml_plugin(QtYARPScopePlugin "${QtYARPScopePlugin_QMLDIR}")

install(TARGETS QtYARPScopePlugin
        COMPONENT utilities
        RUNTIME DESTINATION "${CMAKE_INSTALL_QMLDIR}/${QtYARPScopePlugin_QMLDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_QMLDIR}/${QtYARPScopePlugin_QMLDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_QMLDIR}/${QtYARPScopePlugin_QMLDIR}")

install(FILES qmldir DESTINATION "${CMAKE_INSTALL_QMLDIR}/${QtYARPScopePlugin_QMLDIR}")

set_property(TARGET QtYARPScopePlugin PROPERTY FOLDER "Plugins/QML")
