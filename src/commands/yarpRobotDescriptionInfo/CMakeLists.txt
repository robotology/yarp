# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpRobotDescriptionInfo)

  add_executable(yarpRobotDescriptionInfo)

  set(yarpRobotDescriptionInfo_SRCS
    main.cpp
  )

  set(yarpRobotDescriptionInfo_HDRS
  )

  source_group(
    "Source Files"
    FILES ${yarpRobotDescriptionInfo_SRCS}
  )

  source_group(
    "Header Files"
    FILES ${yarpRobotDescriptionInfo_HDRS}
  )

  target_sources(yarpRobotDescriptionInfo
    PRIVATE
    ${yarpRobotDescriptionInfo_SRCS}
    ${yarpRobotDescriptionInfo_HDRS}
  )

  target_include_directories(yarpRobotDescriptionInfo
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
  )

  target_link_libraries(yarpRobotDescriptionInfo
    PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_dev
  )

  install(
    TARGETS yarpRobotDescriptionInfo
    COMPONENT utilities
    RUNTIME DESTINATION bin
  )

  set_property(TARGET yarpRobotDescriptionInfo PROPERTY FOLDER "Command Line Tools")

endif()
