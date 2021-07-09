# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(wav_test)
  find_package(YARP REQUIRED COMPONENTS sig)
endif()

add_executable(wav_test)
target_sources(wav_test PRIVATE wav_test.cpp)
target_link_libraries(wav_test PRIVATE YARP::YARP_sig)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET wav_test PROPERTY FOLDER "Examples/sig")
  endif()
