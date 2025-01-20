# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpllmgui)
  include(YarpUseQt5)
  include(YarpMacOSUtilities)

  add_executable(yarpllmgui WIN32)

  set(yarpllmgui_SRCS
    main.cpp
    ConversationModel.cpp
    ConversationCallback.cpp
  )

  set(yarpllmgui_HDRS
    ConversationModel.h
    ConversationCallback.h
  )

  set(yarpllmgui_QRC_FILES
    res.qrc
  )

  set(yarpllmgui_QML_FILES
    main.qml
    ConfigurePopup.qml
  )

  qt5_add_resources(yarpllmgui_QRC_GEN_SRCS ${yarpllmgui_QRC_FILES})
  qtyarp_use_qml_plugin()

  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Source Files"
    FILES ${yarpllmgui_SRCS}
  )
  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Header Files"
    FILES ${yarpllmgui_HDRS}
  )
  source_group(
    "Resources Files"
    FILES ${yarpllmgui_QRC_FILES}
  )
  source_group(
    "Generated Files"
    FILES
      ${yarpllmgui_QRC_GEN_SRCS}
   )

  target_sources(yarpllmgui
   PRIVATE
     ${yarpllmgui_SRCS}
     ${yarpllmgui_HDRS}
     ${yarpllmgui_QRC_FILES}
     ${yarpllmgui_QRC_GEN_SRCS}
     ${yarpllmgui_QML_FILES}
  )

  target_include_directories(yarpllmgui PRIVATE "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

  target_link_libraries(yarpllmgui
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_dev
    Qt5::Widgets
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
  )

  install(TARGETS yarpllmgui DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarpllmgui PROPERTY FOLDER "Graphical User Interfaces")

endif()
