# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpAudioPlayer)
  project(yarpAudioPlayer)

  set(yarpAudioPlayer_THRIFT_FILES yarpAudioPlayer.thrift)
  include(YarpChooseIDL)
  yarp_choose_idl(yarpAudioPlayer_THRIFT ${yarpAudioPlayer_THRIFT_FILES})

  file(GLOB folder_source *.cpp)
  file(GLOB folder_header *.h)
  source_group("Source Files"    FILES ${folder_source})
  source_group("Header Files"    FILES ${folder_header})
  source_group("Generated Files" FILES ${yarpAudioPlayer_THRIFT_GEN_FILES})
  source_group("Thrift Files"    FILES ${yarpAudioPlayer_THRIFT_FILES})

  add_executable(yarpAudioPlayer ${folder_source} ${folder_header} ${yarpAudioPlayer_THRIFT_GEN_FILES})

  target_include_directories (yarpAudioPlayer
    PRIVATE
    ${yarpAudioPlayer_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS}
  )

  target_link_libraries(yarpAudioPlayer
    PRIVATE
      YARP::YARP_init
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
  )

  install(TARGETS yarpAudioPlayer COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarpAudioPlayer PROPERTY FOLDER "Command Line Tools")
endif()
