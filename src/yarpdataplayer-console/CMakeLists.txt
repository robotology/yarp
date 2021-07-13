# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpdataplayer)

  add_executable(yarpdataplayer-console)

  set(yarpdataplayer_console_THRIFT_FILES yarpdataplayer_console.thrift)
  include(YarpChooseIDL)
  yarp_choose_idl(yarpdataplayer_console_THRIFT ${yarpdataplayer_console_THRIFT_FILES})

  target_sources(yarpdataplayer-console
    PRIVATE
      main.cpp
      ${yarpdataplayer_console_THRIFT_GEN_FILES}
      ${yarpdataplayer_console_THRIFT_FILES}
  )
  target_include_directories(yarpdataplayer-console
    PRIVATE
      ${yarpdataplayer_console_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS}
  )

  target_link_libraries(yarpdataplayer-console
    PRIVATE
      YARP::YARP_os
      YARP::YARP_init
      YARP::YARP_sig
      YARP::YARP_rosmsg
      YARP::YARP_dataplayer
  )

  install(TARGETS yarpdataplayer-console COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarpdataplayer-console PROPERTY FOLDER "Command Line Tools")
endif()
