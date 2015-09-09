# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qtquick2applicationviewer)

set(yarpscope_qt_SRCS main.cpp
                      qtquick2applicationviewer/qtquick2applicationviewer.cpp)
set(yarpscope_qt_HDRS qtquick2applicationviewer/qtquick2applicationviewer.h)
set(yarpscope_qt_QRC_FILES res.qrc)
set(yarpscope_qt_QML_FILES qml/QtYARPScope/main.qml)

qt5_add_resources(yarpscope_qt_QRC_GEN_SRCS ${yarpscope_qt_QRC_FILES})

source_group("Source Files" FILES ${yarpscope_qt_SRCS})
source_group("Header Files" FILES ${yarpscope_qt_HDRS})
source_group("Resource Files" FILES ${yarpscope_qt_QRC_FILES})
source_group("QML Files" FILES ${yarpscope_qt_QML_FILES})
source_group("Generated Files" FILES ${yarpscope_qt_QRC_GEN_SRCS})

qtyarp_use_qml_plugin()

add_executable(yarpscope-qt WIN32 ${yarpscope_qt_SRCS}
                                  ${yarpscope_qt_HDRS}
                                  ${yarpscope_qt_QRC_GEN_SRCS}
                                  ${yarpscope_qt_QRC_FILES}
                                  ${yarpscope_qt_QML_FILES})

qt5_use_modules(yarpscope-qt Widgets
                             Qml
                             Quick)
qtyarp_deprecate_with_cmake_version(2.8.11)
if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
  target_link_libraries(yarpscope-qt Qt5::WinMain)
endif()

install(TARGETS yarpscope-qt
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

if(NOT YARP_DEFAULT_GTK)
  set_target_properties(yarpscope-qt PROPERTIES OUTPUT_NAME yarpscope)
endif()

if (YARP_OSX_GENERATE_BUNDLES)
    set_target_properties(yarpscope-qt PROPERTIES MACOSX_BUNDLE TRUE)
    # the following two lines do the following thing:
    # 1 - specify the app icon (the name should be enogh i think) in the bundle plist 
    # set_target_properties(yarpscope-qt PROPERTIES MACOSX_BUNDLE_ICON_FILE "AppIcon.icns")
    # 2 - Copy the resource in the Resources folder in the bundle
    # set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    include(YarpOSXBundleUpdateRPATH)
    yarp_osx_bundle_update_rpath(yarpscope-qt)
endif()

