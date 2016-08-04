# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(CREATE_YARPDATAPLAYER)

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
    message(STATUS "yarpdataplayer: OpenCV not selected, keep on building...")
  endif()

  set(yarpdataplayer_SRCS src/aboutdlg.cpp
                          src/genericinfodlg.cpp
                          src/loadingwidget.cpp
                          src/main.cpp
                          src/mainwindow.cpp
                          src/utils.cpp
                          src/worker.cpp)


  set(yarpdataplayer_HDRS include/aboutdlg.h
                          include/genericinfodlg.h
                          include/loadingwidget.h
                          include/log.h
                          include/mainwindow.h
                          include/utils.h
                          include/worker.h)
  if(MSVC)
    list(APPEND yarpdataplayer_HDRS include/msvc/dirent.h)
  endif()
  set(yarpdataplayer_QRC_FILES src/RC/res.qrc)
  set(yarpdataplayer_UI_FILES include/UI/aboutdlg.ui
                              include/UI/genericinfodlg.ui
                              include/UI/loadingwidget.ui
                              include/UI/mainwindow.ui)

  set(yarpdataplayer_THRIFT_FILES yarpdataplayer.thrift)
  if(MSVC)
    set(yarpdataplayer_RC_FILES src/RC/yarpdataplayer_resource.rc)
  endif()

  qt5_add_resources(yarpdataplayer_QRC_GEN_SRCS ${yarpdataplayer_QRC_FILES})
  qt5_wrap_ui(yarpdataplayer_UI_GEN_SRCS ${yarpdataplayer_UI_FILES})

  include(YarpIDL)
  yarp_idl_to_dir(${yarpdataplayer_THRIFT_FILES}
                  ${CMAKE_CURRENT_SOURCE_DIR}/idl_generated_code
                  yarpdataplayer_THRIFT_GEN_SRCS
                  yarpdataplayer_THRIFT_GEN_HDRS
                  yarpdataplayer_THRIFT_INCLUDE_DIRS)

  include_directories(${yarpdataplayer_THRIFT_INCLUDE_DIRS})


  source_group("Source Files" FILES ${yarpdataplayer_SRCS})
  source_group("Header Files" FILES ${yarpdataplayer_HDRS})
  source_group("Resources Files" FILES ${yarpdataplayer_QRC_FILES}
                                       ${yarpdataplayer_RC_FILES})
  source_group("UI Files" FILES ${yarpdataplayer_UI_FILES})
  source_group("Thrift Files" FILES ${yarpdataplayer_THRIFT_FILES})
  source_group("Generated Files" FILES ${yarpdataplayer_QRC_GEN_SRCS}
                                       ${yarpdataplayer_UI_GEN_SRCS}
                                       ${yarpdataplayer_THRIFT_GEN_SRCS}
                                       ${yarpdataplayer_THRIFT_GEN_HDRS})

  add_executable(yarpdataplayer WIN32 ${yarpdataplayer_SRCS}
                                      ${yarpdataplayer_HDRS}
                                      ${yarpdataplayer_QRC_GEN_SRCS}
                                      ${yarpdataplayer_UI_GEN_SRCS}
                                      ${yarpdataplayer_THRIFT_GEN_SRCS}
                                      ${yarpdataplayer_THRIFT_GEN_HDRS}
                                      ${yarpdataplayer_RC_FILES}
                                      ${yarpdataplayer_THRIFT_FILES})

  target_link_libraries(yarpdataplayer YARP_OS
                                       YARP_init
                                       YARP_sig)
  qtyarp_use_modules(yarpdataplayer Widgets)
  qtyarp_deprecate_with_cmake_version(2.8.11)
  if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
    target_link_libraries(yarpdataplayer Qt5::WinMain)
  endif()

  if(YARP_HAS_OPENCV)
    target_link_libraries(yarpdataplayer ${OpenCV_LIBRARIES})
  endif()

  install(TARGETS yarpdataplayer COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  yarp_osx_duplicate_and_add_bundle(TARGET_ORIG yarpdataplayer
                                    TARGET_DEST yarpdataplayer.app
                                    INSTALL_COMPONENT utilities
                                    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")

endif()
