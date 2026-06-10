# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

#if(YARP_COMPILE_yarpconnectionsstats)

  add_executable(yarpconnectionsstats)

  set(yarpconnectionsstats_SRCS
    main.cpp
  )

  set(yarpconnectionsstats_HDRS
  )

  source_group(
    "Source Files"
    FILES ${yarpconnectionsstats_SRCS}
  )

  source_group(
    "Header Files"
    FILES ${yarpconnectionsstats_HDRS}
  )

  target_sources(yarpconnectionsstats
    PRIVATE
    ${yarpconnectionsstats_SRCS}
    ${yarpconnectionsstats_HDRS}
  )

  target_include_directories(yarpconnectionsstats
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
  )

  target_link_libraries(yarpconnectionsstats
    PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_stats
  )

  install(
    TARGETS yarpconnectionsstats
    COMPONENT utilities
    RUNTIME DESTINATION bin
  )

  set_property(TARGET yarpconnectionsstats PROPERTY FOLDER "Command Line Tools")

#endif()
