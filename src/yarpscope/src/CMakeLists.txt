# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

include(YarpOSXUtilities)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qtquick2applicationviewer)

set(yarpscope_SRCS main.cpp
                   qtquick2applicationviewer/qtquick2applicationviewer.cpp)
set(yarpscope_HDRS qtquick2applicationviewer/qtquick2applicationviewer.h)
set(yarpscope_QRC_FILES res.qrc)
set(yarpscope_QML_FILES qml/QtYARPScope/main.qml)

qt5_add_resources(yarpscope_QRC_GEN_SRCS ${yarpscope_QRC_FILES})

source_group("Source Files" FILES ${yarpscope_SRCS})
source_group("Header Files" FILES ${yarpscope_HDRS})
source_group("Resource Files" FILES ${yarpscope_QRC_FILES})
source_group("QML Files" FILES ${yarpscope_QML_FILES})
source_group("Generated Files" FILES ${yarpscope_QRC_GEN_SRCS})

qtyarp_use_qml_plugin()

add_executable(yarpscope WIN32 ${yarpscope_SRCS}
                               ${yarpscope_HDRS}
                               ${yarpscope_QRC_GEN_SRCS}
                               ${yarpscope_QRC_FILES}
                               ${yarpscope_QML_FILES})

qtyarp_use_modules(yarpscope Widgets
                             Qml
                             Quick)
qtyarp_deprecate_with_cmake_version(2.8.11)
if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
  target_link_libraries(yarpscope Qt5::WinMain)
endif()

install(TARGETS yarpscope
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

yarp_osx_duplicate_and_add_bundle(TARGET_ORIG yarpscope
                                  TARGET_DEST yarpscope.app
                                  INSTALL_COMPONENT utilities
                                  INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")
                                  
yarp_install(FILES yarpscope.xml DESTINATION ${YARP_MODULES_INSTALL_DIR})
                                  
