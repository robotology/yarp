# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpframegrabbergui)

  include(YarpUseQt5)
  include(YarpMacOSUtilities)

  add_executable(yarpframegrabbergui WIN32)

  set(yarpframegrabbergui_SRCS  dc1394slider.cpp
                                dc1394sliderwb.cpp
                                dc1394thread.cpp
                                loadingwidget.cpp
                                main.cpp
                                mainwindow.cpp
  )
  set(yarpframegrabbergui_HDRS  dc1394SliderBase.h
                                dc1394slider.h
                                dc1394sliderwb.h
                                dc1394thread.h
                                loadingwidget.h
                                mainwindow.h
  )
  set(yarpframegrabbergui_QRC_FILES  res.qrc
  )
  set(yarpframegrabbergui_UI_FILES   dc1394slider.ui
                                     dc1394sliderwb.ui
                                     loadingwidget.ui
                                     mainwindow.ui
  )

  qt5_add_resources(yarpframegrabbergui_QRC_GEN_SRCS ${yarpframegrabbergui_QRC_FILES})
  qt5_wrap_ui(yarpframegrabbergui_UI_GEN_SRCS ${yarpframegrabbergui_UI_FILES})

  source_group("Source Files" FILES ${yarpframegrabbergui_SRCS})
  source_group("Header Files" FILES ${yarpframegrabbergui_HDRS})
  source_group("Resources Files" FILES ${yarpframegrabbergui_QRC_FILES})
  source_group("UI Files" FILES ${yarpframegrabbergui_UI_FILES})
  source_group("Generated Files" FILES ${yarpframegrabbergui_QRC_GEN_SRCS}
                                      ${yarpframegrabbergui_UI_GEN_SRCS})

  target_sources(yarpframegrabbergui
    PRIVATE
      ${yarpframegrabbergui_SRCS}
      ${yarpframegrabbergui_HDRS}
      ${yarpframegrabbergui_QRC_FILES}
      ${yarpframegrabbergui_QRC_GEN_SRCS}
      ${yarpframegrabbergui_UI_FILES}
      ${yarpframegrabbergui_UI_GEN_SRCS}
  )

  target_include_directories(yarpframegrabbergui
    PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}"
      "${CMAKE_CURRENT_BINARY_DIR}"
  )

  target_link_libraries(yarpframegrabbergui
    PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_dev
    Qt5::Widgets
    Qt5::Gui
  )

  install(
    TARGETS yarpframegrabbergui
    COMPONENT utilities
    DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

  set_property(TARGET yarpframegrabbergui PROPERTY FOLDER "Graphical User Interfaces")

  yarp_macos_duplicate_and_add_bundle(
    TARGET yarpframegrabbergui
    INSTALL_COMPONENT utilities
    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}"
  )
endif()
