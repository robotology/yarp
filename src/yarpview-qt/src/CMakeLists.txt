# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


# Remove -rdynamic from CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS.
# This is needed for some weird reason, or executables will not load resources
# from the plugins.
string(REPLACE "-rdynamic" "" CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS}")


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qtquick2applicationviewer)

set(yarpview_qt_SRCS main.cpp
                     qtquick2applicationviewer/qtquick2applicationviewer.cpp)
set(yarpview_qt_HDRS qtquick2applicationviewer/qtquick2applicationviewer.h)
set(yarpview_qt_QRC_FILES res.qrc)
set(yarpview_qt_QML_FILES qml/QtYARPView/main.qml)

qt5_add_resources(yarpview_qt_QRC_GEN_SRCS ${yarpview_qt_QRC_FILES})

source_group("Source Files" FILES ${yarpview_qt_SRCS})
source_group("Header Files" FILES ${yarpview_qt_HDRS})
source_group("Resources Files" FILES ${yarpview_qt_QRC_FILES})
source_group("QML Files" FILES ${yarpview_qt_QML_FILES})
source_group("Generated Files" FILES ${yarpview_qt_QRC_GEN_SRCS})

add_executable(yarpview-qt WIN32 ${yarpview_qt_SRCS}
                                 ${yarpview_qt_HDRS}
                                 ${yarpview_qt_QRC_GEN_SRCS})
qt5_use_modules(yarpview-qt Widgets
                            Qml
                            Quick)
qtyarp_deprecate_with_cmake_version(2.8.11)
if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
  target_link_libraries(yarpview-qt Qt5::WinMain)
endif()

install(TARGETS yarpview-qt COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

if(NOT YARP_DEFAULT_GTK)
  set_target_properties(yarpview-qt PROPERTIES OUTPUT_NAME yarpview)
endif()
