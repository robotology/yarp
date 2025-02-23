# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(TEST3_THRIFT Test3Msgs.thrift)

add_library(Test3Msgs OBJECT)

target_sources(Test3Msgs PRIVATE ${TEST3_THRIFT_GEN_FILES})

target_link_libraries(Test3Msgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(Test3Msgs PUBLIC ${TEST3_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET Test3Msgs PROPERTY FOLDER "Devices/Shared")
