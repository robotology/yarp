# SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpActionsPlayer)
  project(yarpActionsPlayer)

  file(GLOB folder_source *.cpp)
  file(GLOB folder_header *.h)
  source_group("Source Files" FILES ${folder_source})
  source_group("Header Files" FILES ${folder_header})

  add_executable(yarpActionsPlayer ${folder_source} ${folder_header})

  target_link_libraries(yarpActionsPlayer
    PRIVATE
      YARP::YARP_init
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
  )

  install(TARGETS yarpActionsPlayer COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarpActionsPlayer PROPERTY FOLDER "Command Line Tools")
endif()
