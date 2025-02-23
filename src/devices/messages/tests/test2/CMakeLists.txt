# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(TEST2_THRIFT Test2Msgs.thrift)

add_library(Test2Msgs OBJECT)

target_sources(Test2Msgs PRIVATE ${TEST2_THRIFT_GEN_FILES})

target_link_libraries(Test2Msgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(Test2Msgs PUBLIC ${TEST2_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET Test2Msgs PROPERTY FOLDER "Devices/Shared")
