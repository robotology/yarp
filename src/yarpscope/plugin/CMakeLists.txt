# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
include_directories(${YARP_OS_INCLUDE_DIRS}
                    ${QCustomPlot_INCLUDE_DIRS})

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

source_group("Source Files" FILES ${QtYARPScopePlugin_SRCS})
source_group("Header Files" FILES ${QtYARPScopePlugin_HDRS})
source_group("Resources Files" FILES ${QtYARPScopePlugin_QRC_FILES})
source_group("QML Files" FILES ${QtYARPScopePlugin_QML_FILES})
source_group("Generated Files" FILES ${QtYARPScopePlugin_QRC_GEN_SRCS})

add_library(QtYARPScopePlugin MODULE ${QtYARPScopePlugin_SRCS}
                                     ${QtYARPScopePlugin_HDRS}
                                     ${QtYARPScopePlugin_QRC_GEN_SRCS})
target_link_libraries(QtYARPScopePlugin YARP_OS
                                        YARP_init
                                        ${QCustomPlot_LIBRARIES})
qtyarp_use_modules(QtYARPScopePlugin Widgets
                                     Qml
                                     Quick
                                     Xml
                                     PrintSupport)

qtyarp_qml_plugin(QtYARPScopePlugin "${QtYARPScopePlugin_QMLDIR}")

install(TARGETS QtYARPScopePlugin
        COMPONENT utilities
        RUNTIME DESTINATION "${CMAKE_INSTALL_QMLDIR}/${QtYARPScopePlugin_QMLDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_QMLDIR}/${QtYARPScopePlugin_QMLDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_QMLDIR}/${QtYARPScopePlugin_QMLDIR}")

install(FILES qmldir DESTINATION "${CMAKE_INSTALL_QMLDIR}/${QtYARPScopePlugin_QMLDIR}")
