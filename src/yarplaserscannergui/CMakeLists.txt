# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarplaserscannergui)

  include(YarpUseQt5)
  include(YarpMacOSUtilities)

  add_executable(yarplaserscannergui WIN32)

 # set(yarplaserscannergui_SRCS main.cpp gui.cpp )
 # set(yarplaserscannergui_HDRS gui.h )
 # set(yarplaserscannergui_QRC_FILES res.qrc)
 # set(yarplaserscannergui_UI_FILES gui.ui)

  set(yarplaserscannergui_SRCS main.cpp)

 # qt5_add_resources(yarplaserscannergui_QRC_GEN_SRCS ${yarplaserscannergui_QRC_FILES})
  qt5_wrap_ui(yarplaserscannergui_UI_GEN_SRCS ${yarplaserscannerguii_UI_FILES})

  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Source Files"
    FILES ${yarplaserscannergui_SRCS}
  )
  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Header Files"
    FILES ${yarplaserscannergui_HDRS}
  )
  source_group(
    "Resources Files"
    FILES ${yarplaserscannergui_QRC_FILES}
  )
  source_group(
    "UI Files"
    FILES ${yarplaserscannergui_UI_FILES}
  )
  source_group(
    "Generated Files"
    FILES ${yarpLaserScannergui_QRC_GEN_SRCS}
    ${yarplaserscannergui_UI_GEN_SRCS}
  )

  target_sources(yarplaserscannergui
    PRIVATE
      ${yarplaserscannergui_SRCS}
      ${yarplaserscannergui_HDRS}
      ${yarplaserscannergui_QRC_FILES}
      ${yarplaserscannergui_QRC_GEN_SRCS}
      ${yarplaserscannergui_UI_FILES}
      ${yarplaserscannergui_UI_GEN_SRCS}
  )

  target_link_libraries(yarplaserscannergui
    PRIVATE
      YARP::YARP_os
      YARP::YARP_init
      YARP::YARP_sig
      YARP::YARP_dev
      Qt5::Widgets
      Qt5::Gui
  )

  target_include_directories(yarplaserscannergui PRIVATE ${OpenCV_INCLUDE_DIRS})
  target_link_libraries(yarplaserscannergui PRIVATE ${OpenCV_LIBRARIES})

  install(TARGETS yarplaserscannergui COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarplaserscannergui PROPERTY FOLDER "Graphical User Interfaces")

  yarp_macos_duplicate_and_add_bundle(
    TARGET yarplaserscannergui
    INSTALL_COMPONENT utilities
    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}"
  )

endif()
