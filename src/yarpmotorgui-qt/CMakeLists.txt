# Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


if(CREATE_YARPMOTORGUI AND YARP_HAS_QT5)

  include(YarpUseQt5)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})

  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

  set(yarpmotorgui_qt_SRCS main.cpp
                           flowlayout.cpp
                           jointitem.cpp
                           mainwindow.cpp
                           partitem.cpp
                           piddlg.cpp
                           sequencewindow.cpp
                           startdlg.cpp)
  set(yarpmotorgui_qt_HDRS flowlayout.h
                           jointitem.h
                           log.h
                           mainwindow.h
                           partitem.h
                           piddlg.h
                           yarpmotorgui.h
                           sequencewindow.h
                           startdlg.h)
  set(yarpmotorgui_qt_QRC_FILES res.qrc)
  set(yarpmotorgui_qt_UI_FILES jointitem.ui
                               mainwindow.ui
                               piddlg.ui
                               sequencewindow.ui
                               startdlg.ui)

  qt5_add_resources(yarpmotorgui_qt_QRC_GEN_SRCS ${yarpmotorgui_qt_QRC_FILES})
  qt5_wrap_ui(yarpmotorgui_qt_UI_GEN_SRCS ${yarpmotorgui_qt_UI_FILES})

  source_group("Source Files" FILES ${yarpmotorgui_qt_SRCS})
  source_group("Header Files" FILES ${yarpmotorgui_qt_HDRS})
  source_group("Resources Files" FILES ${yarpmotorgui_qt_QRC_FILES})
  source_group("UI Files" FILES ${yarpmotorgui_qt_UI_FILES})
  source_group("Generated Files" FILES ${yarpmotorgui_qt_QRC_GEN_SRCS}
                                       ${yarpmotorgui_qt_UI_GEN_SRCS})

  add_executable(yarpmotorgui-qt WIN32  ${yarpmotorgui_qt_SRCS}
                                        ${yarpmotorgui_qt_HDRS}
                                        ${yarpmotorgui_qt_QRC_GEN_SRCS}
                                        ${yarpmotorgui_qt_UI_GEN_SRCS})

  if(YARP_OSX_GENERATE_BUNDLES)
    set_target_properties(yarpmotorgui-qt PROPERTIES MACOSX_BUNDLE TRUE)
  endif()

  target_link_libraries(yarpmotorgui-qt YARP_OS
                                         YARP_init
                                         YARP_sig
                                         YARP_dev)
  qt5_use_modules(yarpmotorgui-qt Widgets
                                   Gui)
  qtyarp_deprecate_with_cmake_version(2.8.11)
  if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
    target_link_libraries(yarpmotorgui-qt Qt5::WinMain)
  endif()

  install(TARGETS yarpmotorgui-qt COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  if(NOT YARP_DEFAULT_GTK)
    set_target_properties(yarpmotorgui-qt PROPERTIES OUTPUT_NAME yarpmotorgui)
  endif()

  include(YarpOSXBundleUpdateRPATH)
  yarp_osx_bundle_update_rpath(yarpmotorgui-qt)

endif()
