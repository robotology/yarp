# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(TEST4_THRIFT Test4Msgs.thrift)

add_library(Test4Msgs OBJECT)

target_sources(Test4Msgs PRIVATE ${TEST4_THRIFT_GEN_FILES})

target_link_libraries(Test4Msgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(Test4Msgs PUBLIC ${TEST4_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET Test4Msgs PROPERTY FOLDER "Devices/Shared")
