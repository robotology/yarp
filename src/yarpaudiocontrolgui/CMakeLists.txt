# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpaudiocontrolgui)

  include(YarpUseQt5)
  include(YarpMacOSUtilities)

  add_executable(yarpaudiocontrolgui WIN32)

  set(gui_SRCS
    main.cpp
  )
  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Source Files"
    FILES ${gui_SRCS}
  )

  target_sources(yarpaudiocontrolgui
    PRIVATE
      ${gui_SRCS}
  )

  target_include_directories(yarpaudiocontrolgui
    PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}"
      "${CMAKE_CURRENT_BINARY_DIR}"
  )

  target_link_libraries(yarpaudiocontrolgui
    PRIVATE
      YARP::YARP_os
      YARP::YARP_init
      YARP::YARP_sig
      YARP::YARP_dev
      Qt5::Widgets
      Qt5::Gui
  )

  install(
    TARGETS yarpaudiocontrolgui
    COMPONENT utilities
    DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

  set_property(TARGET yarpaudiocontrolgui PROPERTY FOLDER "Graphical User Interfaces")

  yarp_macos_duplicate_and_add_bundle(
    TARGET yarpaudiocontrolgui
    INSTALL_COMPONENT utilities
    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}"
  )

endif()
