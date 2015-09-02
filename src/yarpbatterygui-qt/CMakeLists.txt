# Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


if(CREATE_YARPBATTERYGUI AND YARP_HAS_QT5)

  include(YarpUseQt5)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})

  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

  set(yarpbatterygui_qt_SRCS main.cpp
                           display.cpp)
  set(yarpbatterygui_qt_HDRS display.h)

  set(yarpbatterygui_qt_QRC_FILES res.qrc)

  set(yarpbatterygui_qt_UI_FILES display.ui)

  qt5_add_resources(yarpbatterygui_qt_QRC_GEN_SRCS ${yarpbatterygui_qt_QRC_FILES})
  
  qt5_wrap_ui(yarpbatterygui_qt_UI_GEN_SRCS ${yarpbatterygui_qt_UI_FILES})

  source_group("Source Files" FILES ${yarpbatterygui_qt_SRCS})
  source_group("Header Files" FILES ${yarpbatterygui_qt_HDRS})
  source_group("Resources Files" FILES ${yarpbatterygui_qt_QRC_FILES})
  source_group("UI Files" FILES ${yarpbatterygui_qt_UI_FILES})
  source_group("Generated Files" FILES ${yarpbatterygui_qt_QRC_GEN_SRCS}
                                      ${yarpbatterygui_qt_UI_GEN_SRCS})

  add_executable(yarpbatterygui-qt WIN32 MACOSX_BUNDLE ${yarpbatterygui_qt_SRCS}
                                        ${yarpbatterygui_qt_HDRS}
                                        ${yarpbatterygui_qt_QRC_GEN_SRCS}
                                        ${yarpbatterygui_qt_UI_GEN_SRCS})
  target_link_libraries(yarpbatterygui-qt YARP_OS
                                         YARP_init
                                         YARP_sig
                                         YARP_dev)
  qt5_use_modules(yarpbatterygui-qt Widgets
                                   Gui)
  qtyarp_deprecate_with_cmake_version(2.8.11)
  if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
    target_link_libraries(yarpbatterygui-qt Qt5::WinMain)
  endif()

  install(TARGETS yarpbatterygui-qt COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  if(NOT YARP_DEFAULT_GTK)
    set_target_properties(yarpbatterygui-qt PROPERTIES OUTPUT_NAME yarpbatterygui)
  endif()

  include(YarpOSXBundleUpdateRPATH)
  yarp_osx_bundle_update_rpath(yarpbatterygui-qt)

endif()
