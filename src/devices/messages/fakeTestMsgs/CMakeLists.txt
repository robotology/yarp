# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(FAKETESTMSGS_THRIFT fakeTestMsgs.thrift)

add_library(fakeTestMsgs OBJECT)

target_sources(fakeTestMsgs PRIVATE ${FAKETESTMSGS_THRIFT_GEN_FILES})

target_link_libraries(fakeTestMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(fakeTestMsgs PUBLIC ${FAKETESTMSGS_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET fakeTestMsgs PROPERTY FOLDER "Devices/Shared")
