# Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(CREATE_YARPBATTERYGUI)

  include(YarpUseQt5)
  include(YarpOSXUtilities)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})

  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

  set(yarpbatterygui_SRCS main.cpp
                          display.cpp)
  set(yarpbatterygui_HDRS display.h)

  set(yarpbatterygui_QRC_FILES res.qrc)

  set(yarpbatterygui_UI_FILES display.ui)

  qt5_add_resources(yarpbatterygui_QRC_GEN_SRCS ${yarpbatterygui_QRC_FILES})

  qt5_wrap_ui(yarpbatterygui_UI_GEN_SRCS ${yarpbatterygui_UI_FILES})

  source_group("Source Files" FILES ${yarpbatterygui_SRCS})
  source_group("Header Files" FILES ${yarpbatterygui_HDRS})
  source_group("Resources Files" FILES ${yarpbatterygui_QRC_FILES})
  source_group("UI Files" FILES ${yarpbatterygui_UI_FILES})
  source_group("Generated Files" FILES ${yarpbatterygui_QRC_GEN_SRCS}
                                       ${yarpbatterygui_UI_GEN_SRCS})

  add_executable(yarpbatterygui WIN32 ${yarpbatterygui_SRCS}
                                      ${yarpbatterygui_HDRS}
                                      ${yarpbatterygui_QRC_GEN_SRCS}
                                      ${yarpbatterygui_UI_GEN_SRCS})

  target_link_libraries(yarpbatterygui YARP_OS
                                       YARP_init
                                       YARP_sig
                                       YARP_dev)
  qtyarp_use_modules(yarpbatterygui Widgets
                                    Gui)
  qtyarp_deprecate_with_cmake_version(2.8.11)
  if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
    target_link_libraries(yarpbatterygui Qt5::WinMain)
  endif()

  install(TARGETS yarpbatterygui COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  yarp_osx_duplicate_and_add_bundle(TARGET_ORIG yarpbatterygui
                                    TARGET_DEST yarpbatterygui.app
                                    INSTALL_COMPONENT utilities
                                    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")

endif()
