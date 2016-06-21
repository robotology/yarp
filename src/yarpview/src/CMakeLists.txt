# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

include(YarpOSXUtilities)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qtquick2applicationviewer)

set(yarpview_SRCS main.cpp
                  qtquick2applicationviewer/qtquick2applicationviewer.cpp)
set(yarpview_HDRS qtquick2applicationviewer/qtquick2applicationviewer.h)
set(yarpview_QRC_FILES res.qrc)
set(yarpview_QML_FILES qml/QtYARPView/main.qml qml/QtYARPView/mainCompact.qml)

qt5_add_resources(yarpview_QRC_GEN_SRCS ${yarpview_QRC_FILES})

source_group("Source Files" FILES ${yarpview_SRCS})
source_group("Header Files" FILES ${yarpview_HDRS})
source_group("Resources Files" FILES ${yarpview_QRC_FILES})
source_group("QML Files" FILES ${yarpview_QML_FILES})
source_group("Generated Files" FILES ${yarpview_QRC_GEN_SRCS})

qtyarp_use_qml_plugin()

add_executable(yarpview WIN32 ${yarpview_SRCS}
                              ${yarpview_HDRS}
                              ${yarpview_QRC_GEN_SRCS}
                              ${yarpview_QRC_FILES}
                              ${yarpview_QML_FILES})

qtyarp_use_modules(yarpview Widgets
                            Qml
                            Quick)
qtyarp_deprecate_with_cmake_version(2.8.11)
if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
  target_link_libraries(yarpview Qt5::WinMain)
endif()

install(TARGETS yarpview
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

yarp_osx_duplicate_and_add_bundle(TARGET_ORIG yarpview
                                  TARGET_DEST yarpview.app
                                  APP_ICON icons/AppIcon.icns
                                  INSTALL_COMPONENT utilities
                                  INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")

if(APPLE)
  add_dependencies(yarpview.app QtYARPViewPlugin)
  add_custom_command(TARGET yarpview.app POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:QtYARPViewPlugin>"
                                                      "$<TARGET_FILE_DIR:yarpview.app>/../lib/qt5/qml/${QtYARPViewPlugin_QMLDIR}/$<TARGET_FILE_NAME:QtYARPViewPlugin>"
                     COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:QtYARPViewPlugin>/qmldir"
                                                      "$<TARGET_FILE_DIR:yarpview.app>/../lib/qt5/qml/${QtYARPViewPlugin_QMLDIR}/qmldir")
endif()

yarp_install(FILES yarpview.xml
             DESTINATION ${YARP_MODULES_INSTALL_DIR})
