# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include(YarpMacOSUtilities)

add_executable(yarpview WIN32)

set(yarpview_SRCS main.cpp
                  qtquick2applicationviewer/qtquick2applicationviewer.cpp)
set(yarpview_HDRS qtquick2applicationviewer/qtquick2applicationviewer.h)
set(yarpview_QRC_FILES res.qrc)
set(yarpview_QML_FILES qml/QtYARPView/main.qml qml/QtYARPView/mainCompact.qml)

qt5_add_resources(yarpview_QRC_GEN_SRCS ${yarpview_QRC_FILES})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${yarpview_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${yarpview_HDRS})
source_group("Resources Files" FILES ${yarpview_QRC_FILES})
source_group("QML Files" FILES ${yarpview_QML_FILES})
source_group("Generated Files" FILES ${yarpview_QRC_GEN_SRCS})

qtyarp_use_qml_plugin()

target_sources(yarpview PRIVATE ${yarpview_SRCS}
                                ${yarpview_HDRS}
                                ${yarpview_QRC_FILES}
                                ${yarpview_QRC_GEN_SRCS}
                                ${yarpview_QML_FILES})

target_include_directories(yarpview PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/qtquick2applicationviewer)

target_link_libraries(yarpview Qt5::Widgets
                               Qt5::Qml
                               Qt5::Quick)

install(TARGETS yarpview
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

set_property(TARGET yarpview PROPERTY FOLDER "Graphical User Interfaces")

yarp_macos_duplicate_and_add_bundle(TARGET yarpview
                                    APP_ICON icons/AppIcon.icns
                                    INSTALL_COMPONENT utilities
                                    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")

if(APPLE AND NOT YARP_DISABLE_MACOS_BUNDLES)
  add_dependencies(yarpview.app QtYARPViewPlugin)
  add_custom_command(TARGET yarpview.app POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:QtYARPViewPlugin>"
                                                      "$<TARGET_FILE_DIR:yarpview.app>/../lib/qt5/qml/${QtYARPViewPlugin_QMLDIR}/$<TARGET_FILE_NAME:QtYARPViewPlugin>"
                     COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:QtYARPViewPlugin>/qmldir"
                                                      "$<TARGET_FILE_DIR:yarpview.app>/../lib/qt5/qml/${QtYARPViewPlugin_QMLDIR}/qmldir")
endif()

yarp_install(FILES yarpview.xml
             DESTINATION ${YARP_MODULES_INSTALL_DIR})
