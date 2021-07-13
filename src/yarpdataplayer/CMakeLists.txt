# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpdataplayer)

  include(YarpUseQt5)
  include(YarpMacOSUtilities)

  add_executable(yarpdataplayer WIN32)

  set(yarpdataplayer_SRCS
    src/aboutdlg.cpp
    src/genericinfodlg.cpp
    src/loadingwidget.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/utils.cpp
    src/worker.cpp
  )


  set(yarpdataplayer_HDRS
    include/aboutdlg.h
    include/genericinfodlg.h
    include/loadingwidget.h
    include/mainwindow.h
    include/utils.h
    include/worker.h
  )

  set(yarpdataplayer_QRC_FILES
    src/RC/res.qrc
  )

  set(yarpdataplayer_UI_FILES
    include/UI/aboutdlg.ui
    include/UI/genericinfodlg.ui
    include/UI/loadingwidget.ui
    include/UI/mainwindow.ui
  )

  set(yarpdataplayer_THRIFT_FILES yarpdataplayer.thrift)

  if(MSVC)
    set(yarpdataplayer_RC_FILES src/RC/yarpdataplayer_resource.rc)
  endif()

  qt5_add_resources(yarpdataplayer_QRC_GEN_SRCS ${yarpdataplayer_QRC_FILES})
  qt5_wrap_ui(yarpdataplayer_UI_GEN_SRCS ${yarpdataplayer_UI_FILES})

  include(YarpChooseIDL)
  yarp_choose_idl(yarpdataplayer_THRIFT ${yarpdataplayer_THRIFT_FILES})

  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Source Files"
    FILES ${yarpdataplayer_SRCS}
  )
  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Header Files"
    FILES ${yarpdataplayer_HDRS}
  )
  source_group(
    "Resources Files"
    FILES
      ${yarpdataplayer_QRC_FILES}
      ${yarpdataplayer_RC_FILES}
  )
  source_group(
    "UI Files"
      FILES ${yarpdataplayer_UI_FILES}
  )
  source_group(
    "Thrift Files"
      FILES ${yarpdataplayer_THRIFT_FILES}
  )
  source_group(
    "Generated Files"
      FILES
        ${yarpdataplayer_QRC_GEN_SRCS}
        ${yarpdataplayer_UI_GEN_SRCS}
        ${yarpdataplayer_THRIFT_GEN_FILES}
  )

  target_sources(yarpdataplayer
    PRIVATE
      ${yarpdataplayer_SRCS}
      ${yarpdataplayer_HDRS}
      ${yarpdataplayer_QRC_FILES}
      ${yarpdataplayer_QRC_GEN_SRCS}
      ${yarpdataplayer_UI_FILES}
      ${yarpdataplayer_UI_GEN_SRCS}
      ${yarpdataplayer_THRIFT_GEN_FILES}
      ${yarpdataplayer_RC_FILES}
      ${yarpdataplayer_THRIFT_FILES}
  )

  target_include_directories(yarpdataplayer
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
      ${yarpdataplayer_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS}
  )

  target_link_libraries(yarpdataplayer
    PRIVATE
      YARP::YARP_os
      YARP::YARP_init
      YARP::YARP_sig
      YARP::YARP_dataplayer
      YARP::YARP_rosmsg
      Qt5::Widgets
  )

  if(YARP_HAS_OpenCV)
    target_compile_definitions(yarpdataplayer PRIVATE HAS_OPENCV)
    target_include_directories(yarpdataplayer PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(yarpdataplayer
      PRIVATE
        ${OpenCV_LIBRARIES}
        YARP::YARP_cv
    )
  endif()

  if(MSVC)
    target_include_directories(yarpdataplayer SYSTEM PRIVATE ${dirent_INCLUDE_DIRS})
  endif()

  install(TARGETS yarpdataplayer COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarpdataplayer PROPERTY FOLDER "Graphical User Interfaces")

  yarp_macos_duplicate_and_add_bundle(
    TARGET yarpdataplayer
    INSTALL_COMPONENT utilities
    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}"
  )

endif()
