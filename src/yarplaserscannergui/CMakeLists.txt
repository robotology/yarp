# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarplaserscannergui)

  include(YarpMacOSUtilities)
  add_executable(yarplaserscannergui)

  set(yarplaserscannergui_SRCS main.cpp)

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

  target_sources(yarplaserscannergui
    PRIVATE
      ${yarplaserscannergui_SRCS}
      ${yarplaserscannergui_HDRS}
  )

  target_link_libraries(yarplaserscannergui
    PRIVATE
      YARP::YARP_os
      YARP::YARP_init
      YARP::YARP_sig
      YARP::YARP_dev
  )

  target_include_directories(yarplaserscannergui PRIVATE ${OpenCV_INCLUDE_DIRS})
  target_link_libraries(yarplaserscannergui PRIVATE ${OpenCV_LIBS})

  install(TARGETS yarplaserscannergui COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarplaserscannergui PROPERTY FOLDER "Graphical User Interfaces")

  yarp_macos_duplicate_and_add_bundle(
    TARGET yarplaserscannergui
    INSTALL_COMPONENT utilities
    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}"
  )

endif()
