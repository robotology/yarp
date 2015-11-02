# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(CREATE_YARPDATAPLAYER AND YARP_HAS_QT5)

  include(YarpUseQt5)
  include(YarpOSXUtilities)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)

  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS})

  if(YARP_HAS_OPENCV)
    add_definitions(-DHAS_OPENCV)
    include_directories(${OpenCV_INCLUDE_DIRS})
  else()
    message(STATUS "yarpdataplayer-qt: OpenCV not selected, keep on building...")
  endif()

  set(yarpdataplayer_qt_SRCS src/aboutdlg.cpp
                             src/genericinfodlg.cpp
                             src/loadingwidget.cpp
                             src/main.cpp
                             src/mainwindow.cpp
                             src/utils.cpp
                             src/worker.cpp)


  set(yarpdataplayer_qt_HDRS include/aboutdlg.h
                             include/genericinfodlg.h
                             include/loadingwidget.h
                             include/log.h
                             include/mainwindow.h
                             include/utils.h
                             include/worker.h)
  if(MSVC)
    list(APPEND yarpdataplayer_HDRS include/msvc/dirent.h)
  endif()
  set(yarpdataplayer_qt_QRC_FILES src/RC/res.qrc)
  set(yarpdataplayer_qt_UI_FILES include/UI/aboutdlg.ui
                                 include/UI/genericinfodlg.ui
                                 include/UI/loadingwidget.ui
                                 include/UI/mainwindow.ui)

  set(yarpdataplayer_qt_THRIFT_FILES yarpdataplayer.thrift)
  if(MSVC)
    set(yarpdataplayer_qt_RC_FILES src/RC/yarpdataplayer_resource.rc)
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
                                         ${yarpdataplayer_qt_RC_FILES}
                                         ${yarpdataplayer_qt_THRIFT_FILES})

  target_link_libraries(yarpdataplayer-qt YARP_OS
                                          YARP_init
                                          YARP_sig)
  qtyarp_use_modules(yarpdataplayer-qt Widgets)
  qtyarp_deprecate_with_cmake_version(2.8.11)
  if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
    target_link_libraries(yarpdataplayer-qt Qt5::WinMain)
  endif()

  if(YARP_HAS_OPENCV)
    target_link_libraries(yarpdataplayer-qt ${OpenCV_LIBRARIES})
  endif()

  install(TARGETS yarpdataplayer-qt COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  if(NOT YARP_DEFAULT_GTK)
    set_target_properties(yarpdataplayer-qt PROPERTIES OUTPUT_NAME yarpdataplayer)
  endif()

  yarp_osx_duplicate_and_add_bundle(TARGET_ORIG yarpdataplayer-qt
                                    TARGET_DEST yarpdataplayer-qt.app
                                    INSTALL_COMPONENT utilities
                                    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")

endif()
