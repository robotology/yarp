# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpconnectionsinfo)

  add_executable(yarpconnectionsinfo)

  set(yarpconnectionsinfo_SRCS
    main.cpp
  )

  set(yarpconnectionsinfo_HDRS
  )

  source_group(
    "Source Files"
    FILES ${yarpconnectionsinfo_SRCS}
  )

  source_group(
    "Header Files"
    FILES ${yarpconnectionsinfo_HDRS}
  )

  target_sources(yarpconnectionsinfo
    PRIVATE
    ${yarpconnectionsinfo_SRCS}
    ${yarpconnectionsinfo_HDRS}
  )

  target_include_directories(yarpconnectionsinfo
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
  )

  target_link_libraries(yarpconnectionsinfo
    PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_profiler
  )

  install(
    TARGETS yarpconnectionsinfo
    COMPONENT utilities
    RUNTIME DESTINATION bin
  )

  set_property(TARGET yarpconnectionsinfo PROPERTY FOLDER "Command Line Tools")

endif()
