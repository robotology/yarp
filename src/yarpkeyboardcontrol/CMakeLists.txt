# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(YARP_COMPILE_yarpkeyboardcontrol)

  include(YarpUseQt5)
  include(YarpMacOSUtilities)

  add_executable(yarpkeyboardcontrol WIN32)

  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

  set(yarpkeyboardcontrol_SRCS main.cpp
                          display.cpp)
  set(yarpkeyboardcontrol_HDRS display.h)

  set(yarpkeyboardcontrol_QRC_FILES res.qrc)

  set(yarpkeyboardcontrol_UI_FILES display.ui)

  qt5_add_resources(yarpkeyboardcontrol_QRC_GEN_SRCS ${yarpkeyboardcontrol_QRC_FILES})

  qt5_wrap_ui(yarpkeyboardcontrol_UI_GEN_SRCS ${yarpkeyboardcontrol_UI_FILES})

  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
               PREFIX "Source Files"
               FILES ${yarpkeyboardcontrol_SRCS})
  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
               PREFIX "Header Files"
               FILES ${yarpkeyboardcontrol_HDRS})
  source_group("Resources Files" FILES ${yarpkeyboardcontrol_QRC_FILES})
  source_group("UI Files" FILES ${yarpkeyboardcontrol_UI_FILES})
  source_group("Generated Files" FILES ${yarpkeyboardcontrol_QRC_GEN_SRCS}
                                       ${yarpkeyboardcontrol_UI_GEN_SRCS})

  target_sources(yarpkeyboardcontrol PRIVATE ${yarpkeyboardcontrol_SRCS}
                                        ${yarpkeyboardcontrol_HDRS}
                                        ${yarpkeyboardcontrol_QRC_FILES}
                                        ${yarpkeyboardcontrol_QRC_GEN_SRCS}
                                        ${yarpkeyboardcontrol_UI_FILES}
                                        ${yarpkeyboardcontrol_UI_GEN_SRCS})

  target_link_libraries(yarpkeyboardcontrol PRIVATE YARP::YARP_os
                                               YARP::YARP_init
                                               YARP::YARP_sig
                                               YARP::YARP_dev
                                               Qt5::Widgets
                                               Qt5::Gui)

  install(TARGETS yarpkeyboardcontrol
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarpkeyboardcontrol PROPERTY FOLDER "Graphical User Interfaces")

  yarp_macos_duplicate_and_add_bundle(TARGET yarpkeyboardcontrol
                                      INSTALL_COMPONENT utilities
                                      INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")

endif()
