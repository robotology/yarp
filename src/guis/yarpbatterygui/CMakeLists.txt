# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpbatterygui)

  include(YarpUseQt5)
  include(YarpMacOSUtilities)

  add_executable(yarpbatterygui WIN32)

  set(yarpbatterygui_SRCS
    main.cpp
    display.cpp
  )
  set(yarpbatterygui_HDRS
    display.h
  )

  set(yarpbatterygui_QRC_FILES res.qrc)

  set(yarpbatterygui_UI_FILES display.ui)

  qt5_add_resources(yarpbatterygui_QRC_GEN_SRCS ${yarpbatterygui_QRC_FILES})

  qt5_wrap_ui(yarpbatterygui_UI_GEN_SRCS ${yarpbatterygui_UI_FILES})

  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Source Files"
    FILES ${yarpbatterygui_SRCS}
  )
  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Header Files"
    FILES ${yarpbatterygui_HDRS}
  )
  source_group(
    "Resources Files"
    FILES ${yarpbatterygui_QRC_FILES}
  )
  source_group(
    "UI Files"
    FILES ${yarpbatterygui_UI_FILES}
  )
  source_group(
    "Generated Files"
    FILES
      ${yarpbatterygui_QRC_GEN_SRCS}
      ${yarpbatterygui_UI_GEN_SRCS}
  )

  target_sources(yarpbatterygui
    PRIVATE
      ${yarpbatterygui_SRCS}
      ${yarpbatterygui_HDRS}
      ${yarpbatterygui_QRC_FILES}
      ${yarpbatterygui_QRC_GEN_SRCS}
      ${yarpbatterygui_UI_FILES}
      ${yarpbatterygui_UI_GEN_SRCS}
  )

  target_include_directories(yarpbatterygui
    PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}"
      "${CMAKE_CURRENT_BINARY_DIR}"
  )

  target_link_libraries(yarpbatterygui
    PRIVATE
      YARP::YARP_os
      YARP::YARP_init
      YARP::YARP_sig
      YARP::YARP_dev
      Qt5::Widgets
      Qt5::Gui
  )

  install(
    TARGETS yarpbatterygui
    COMPONENT utilities
    DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

  set_property(TARGET yarpbatterygui PROPERTY FOLDER "Graphical User Interfaces")

  yarp_macos_duplicate_and_add_bundle(
    TARGET yarpbatterygui
    INSTALL_COMPONENT utilities
    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}"
  )

endif()
