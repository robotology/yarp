# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.19)

  project(yarp-rerun)

  find_package(YARP REQUIRED)

  include(GNUInstallDirs)
endif()

find_package(rerun_sdk QUIET)
if(NOT rerun_sdk_FOUND)
    message(STATUS  "rerun_sdk not found. Skipping yarpRerun.")
    return()
endif()


add_executable(yarpRerun)
target_sources(yarpRerun
  PRIVATE
    yarpRerun.cpp
)

target_link_libraries(yarpRerun
  PRIVATE
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_dev
    YARP::YARP_init
    rerun_sdk
)

install(
  TARGETS yarpRerun
  COMPONENT utilities
  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set_property(TARGET yarpRerun PROPERTY FOLDER "Command Line Tools")
