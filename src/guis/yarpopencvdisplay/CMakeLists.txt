# SPDX-FileCopyrightText: 2024 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpopencvdisplay)

  include(YarpMacOSUtilities)
  add_executable(yarpopencvdisplay)

  set(yarpopencvdisplay_SRCS main.cpp)

  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Source Files"
    FILES ${yarpopencvdisplay_SRCS}
  )
  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Header Files"
    FILES ${yarpopencvdisplay_HDRS}
  )


  target_sources(yarpopencvdisplay
    PRIVATE
      ${yarpopencvdisplay_SRCS}
      ${yarpopencvdisplay_HDRS}
  )

  target_link_libraries(yarpopencvdisplay
    PRIVATE
      YARP::YARP_os
      YARP::YARP_init
      YARP::YARP_sig
      YARP::YARP_dev
  )

  target_include_directories(yarpopencvdisplay PRIVATE ${OpenCV_INCLUDE_DIRS})
  target_link_libraries(yarpopencvdisplay PRIVATE ${OpenCV_LIBS})

  install(TARGETS yarpopencvdisplay COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarpopencvdisplay PROPERTY FOLDER "Graphical User Interfaces")

  yarp_macos_duplicate_and_add_bundle(
    TARGET yarpopencvdisplay
    INSTALL_COMPONENT utilities
    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}"
  )

endif()
