# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpmobilebasegui)

  include(YarpUseQt5)
  include(YarpMacOSUtilities)

  add_executable(yarpmobilebasegui WIN32)

  set(yarpmobilebasegui_SRCS
    main.cpp
    display.cpp
    joywidget.cpp
  )
  set(yarpmobilebasegui_HDRS
    display.h
    joywidget.h
  )
  set(yarpmobilebasegui_QRC_FILES
    res.qrc
  )

  set(yarpmobilebasegui_UI_FILES
    display.ui
  )

  qt5_add_resources(yarpmobilebasegui_QRC_GEN_SRCS ${yarpmobilebasegui_QRC_FILES})

  qt5_wrap_ui(yarpmobilebasegui_UI_GEN_SRCS ${yarpmobilebasegui_UI_FILES})

  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Source Files"
    FILES ${yarpmobilebasegui_SRCS}
  )
  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Header Files"
    FILES ${yarpmobilebasegui_HDRS}
  )
  source_group(
    "Resources Files"
    FILES ${yarpmobilebasegui_QRC_FILES}
  )
  source_group(
    "UI Files"
    FILES ${yarpmobilebasegui_UI_FILES}
  )
  source_group(
    "Generated Files"
    FILES
      ${yarpmobilebasegui_QRC_GEN_SRCS}
      ${yarpmobilebasegui_UI_GEN_SRCS}
  )

  target_sources(yarpmobilebasegui
    PRIVATE
      ${yarpmobilebasegui_SRCS}
      ${yarpmobilebasegui_HDRS}
      ${yarpmobilebasegui_QRC_FILES}
      ${yarpmobilebasegui_QRC_GEN_SRCS}
      ${yarpmobilebasegui_UI_FILES}
      ${yarpmobilebasegui_UI_GEN_SRCS}
  )

  target_include_directories(yarpmobilebasegui
    PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}"
      "${CMAKE_CURRENT_BINARY_DIR}"
  )

  target_link_libraries(yarpmobilebasegui
    PRIVATE
      YARP::YARP_os
      YARP::YARP_init
      YARP::YARP_sig
      YARP::YARP_dev
      Qt5::Widgets
      Qt5::Gui
  )

  install(
    TARGETS yarpmobilebasegui
    COMPONENT utilities
    DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

  set_property(TARGET yarpmobilebasegui PROPERTY FOLDER "Graphical User Interfaces")

  yarp_macos_duplicate_and_add_bundle(
    TARGET yarpmobilebasegui
    INSTALL_COMPONENT utilities
    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}"
  )

endif()
