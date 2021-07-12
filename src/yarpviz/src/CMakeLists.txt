# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause


set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(yarpviz WIN32)

set(yarpviz_qt_SRCS main.cpp
                    MainWindow.cpp
                    QGraphicsViewEc.cpp
                    informationdialog.cpp
                    qosconfigdialog.cpp
                    portloggerdialog.cpp
                    batchqosconfdialog.cpp)

set(yarpviz_qt_HDRS MainWindow.h
                    QGraphicsViewEc.h
                    informationdialog.h
                    qosconfigdialog.h
                    portloggerdialog.h
                    batchqosconfdialog.h)

set(yarpviz_qt_QRC_FILES res.qrc)

set(yarpviz_qt_UI_FILES MainWindow.ui
                        informationdialog.ui
                        qosconfigdialog.ui
                        portloggerdialog.ui
                        batchqosconfdialog.ui)


qt5_add_resources(yarpviz_qt_QRC_GEN_SRCS ${yarpviz_qt_QRC_FILES})
qt5_wrap_ui(yarpviz_qt_UI_GEN_SRCS ${yarpviz_qt_UI_FILES})

source_group("Source Files" FILES ${yarpviz_qt_SRCS})
source_group("Header Files" FILES ${yarpviz_qt_HDRS})
source_group("Resources Files" FILES ${yarpviz_qt_QRC_FILES})
source_group("UI Files" FILES ${yarpviz_qt_UI_FILES})
source_group("Generated Files" FILES ${yarpviz_qt_QRC_GEN_SRCS}
                                  ${yarpviz_qt_UI_GEN_SRCS})

target_sources(yarpviz PRIVATE ${yarpviz_qt_SRCS}
                               ${yarpviz_qt_HDRS}
                               ${yarpviz_qt_QRC_FILES}
                               ${yarpviz_qt_QRC_GEN_SRCS}
                               ${yarpviz_qt_UI_GEN_SRCS}
                               ${yarpviz_qt_UI_FILES})

target_link_libraries(yarpviz PRIVATE YARP::YARP_os
                                      YARP::YARP_init
                                      YARP::YARP_profiler
                                      Qt5::Widgets
                                      Qt5::Gui)

target_include_directories(yarpviz SYSTEM PRIVATE ${QGVCore_INCLUDE_DIRS})
target_link_libraries(yarpviz PRIVATE ${QGVCore_LIBRARIES})

install(TARGETS yarpviz
        COMPONENT utilities
        RUNTIME DESTINATION bin)


set_property(TARGET yarpviz PROPERTY FOLDER "Graphical User Interfaces")

yarp_macos_duplicate_and_add_bundle(TARGET yarpviz
                                    INSTALL_COMPONENT utilities
                                    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")
