# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include(YarpMacOSUtilities)

add_executable(yarpscope WIN32)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qtquick2applicationviewer)

set(yarpscope_SRCS main.cpp
                   qtquick2applicationviewer/qtquick2applicationviewer.cpp)
set(yarpscope_HDRS qtquick2applicationviewer/qtquick2applicationviewer.h)
set(yarpscope_QRC_FILES res.qrc)
set(yarpscope_QML_FILES qml/QtYARPScope/main.qml)

qt5_add_resources(yarpscope_QRC_GEN_SRCS ${yarpscope_QRC_FILES})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${yarpscope_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${yarpscope_HDRS})
source_group("Resources Files" FILES ${yarpscope_QRC_FILES})
source_group("QML Files" FILES ${yarpscope_QML_FILES})
source_group("Generated Files" FILES ${yarpscope_QRC_GEN_SRCS})

qtyarp_use_qml_plugin()

target_sources(yarpscope PRIVATE ${yarpscope_SRCS}
                                 ${yarpscope_HDRS}
                                 ${yarpscope_QRC_GEN_SRCS}
                                 ${yarpscope_QRC_FILES}
                                 ${yarpscope_QML_FILES})

target_link_libraries(yarpscope PRIVATE Qt5::Widgets
                                        Qt5::Qml
                                        Qt5::Quick)

install(TARGETS yarpscope
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

set_property(TARGET yarpscope PROPERTY FOLDER "Graphical User Interfaces")

yarp_macos_duplicate_and_add_bundle(TARGET yarpscope
                                    INSTALL_COMPONENT utilities
                                    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")

if(APPLE AND NOT YARP_DISABLE_MACOS_BUNDLES)
  add_dependencies(yarpscope.app QtYARPScopePlugin)
  add_custom_command(TARGET yarpscope.app POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:QtYARPScopePlugin>"
                                                      "$<TARGET_FILE_DIR:yarpscope.app>/../lib/qt5/qml/${QtYARPScopePlugin_QMLDIR}/$<TARGET_FILE_NAME:QtYARPScopePlugin>"
                     COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:QtYARPScopePlugin>/qmldir"
                                                      "$<TARGET_FILE_DIR:yarpscope.app>/../lib/qt5/qml/${QtYARPScopePlugin_QMLDIR}/qmldir")
endif()

yarp_install(FILES yarpscope.xml DESTINATION ${YARP_MODULES_INSTALL_DIR})
