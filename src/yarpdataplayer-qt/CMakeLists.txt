# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(CREATE_YARPDATAPLAYER AND YARP_HAS_QT5 AND YARP_HAS_OPENCV)

  include(YarpUseQt5)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)

  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS})

  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

  set(yarpdataplayer_qt_SRCS aboutdlg.cpp
                             genericinfodlg.cpp
                             loadingwidget.cpp
                             main.cpp
                             mainwindow.cpp
                             utils.cpp
                             worker.cpp)
  set(yarpdataplayer_qt_HDRS aboutdlg.h
                             genericinfodlg.h
                             loadingwidget.h
                             log.h
                             mainwindow.h
                             utils.h
                             worker.h)
  if(MSVC)
    list(APPEND yarpdataplayer_HDRS include/msvc/dirent.h)
  endif()
  set(yarpdataplayer_qt_QRC_FILES res.qrc)
  set(yarpdataplayer_qt_UI_FILES aboutdlg.ui
                                 genericinfodlg.ui
                                 loadingwidget.ui
                                 mainwindow.ui)
  set(yarpdataplayer_qt_THRIFT_FILES dataSetPlayer.thrift)
  if(MSVC)
    set(yarpdataplayer_qt_RC_FILES QtDataSetPlayer_resource.rc)
  endif()

  qt5_add_resources(yarpdataplayer_qt_QRC_GEN_SRCS ${yarpdataplayer_qt_QRC_FILES})
  qt5_wrap_ui(yarpdataplayer_qt_UI_GEN_SRCS ${yarpdataplayer_qt_UI_FILES})

  include(YarpIDL)
  yarp_idl_to_dir(${yarpdataplayer_qt_THRIFT_FILES}
                  ${CMAKE_CURRENT_SOURCE_DIR}/idl_generated_code
                  yarpdataplayer_qt_THRIFT_GEN_SRCS
                  yarpdataplayer_qt_THRIFT_GEN_HDRS
                  yarpdataplayer_qt_THRIFT_INCLUDE_DIRS)
  include_directories(${yarpdataplayer_qt_THRIFT_INCLUDE_DIRS})


  source_group("Source Files" FILES ${yarpdataplayer_qt_SRCS})
  source_group("Header Files" FILES ${yarpdataplayer_qt_HDRS})
  source_group("Resources Files" FILES ${yarpdataplayer_qt_QRC_FILES}
                                       ${yarpdataplayer_qt_RC_FILES})
  source_group("UI Files" FILES ${yarpdataplayer_qt_UI_FILES})
  source_group("Thrift Files" FILES ${yarpdataplayer_qt_THRIFT_FILES})
  source_group("Generated Files" FILES ${yarpdataplayer_qt_QRC_GEN_SRCS}
                                       ${yarpdataplayer_qt_UI_GEN_SRCS}
                                       ${yarpdataplayer_qt_THRIFT_GEN_SRCS}
                                       ${yarpdataplayer_qt_THRIFT_GEN_HDRS})

  add_executable(yarpdataplayer-qt WIN32 ${yarpdataplayer_qt_SRCS}
                                         ${yarpdataplayer_qt_HDRS}
                                         ${yarpdataplayer_qt_QRC_GEN_SRCS}
                                         ${yarpdataplayer_qt_UI_GEN_SRCS}
                                         ${yarpdataplayer_qt_THRIFT_GEN_SRCS}
                                         ${yarpdataplayer_qt_THRIFT_GEN_HDRS}
                                         ${yarpdataplayer_qt_RC_FILES})

  target_link_libraries(yarpdataplayer-qt YARP_OS
                                          YARP_init
                                          YARP_sig)
  qt5_use_modules(yarpdataplayer-qt Widgets)
  qtyarp_deprecate_with_cmake_version(2.8.11)
  if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
    target_link_libraries(yarpdataplayer-qt Qt5::WinMain)
  endif()

  install(TARGETS yarpdataplayer-qt COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  if(NOT YARP_DEFAULT_GTK)
    set_target_properties(yarpdataplayer-qt PROPERTIES OUTPUT_NAME yarpdataplayer)
  endif()

endif()
