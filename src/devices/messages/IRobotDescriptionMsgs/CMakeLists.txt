# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(IROBDESC_THRIFT IRobotDescriptionMsgs.thrift)

add_library(IRobotDescriptionMsgs OBJECT)

target_sources(IRobotDescriptionMsgs PRIVATE ${IROBDESC_THRIFT_GEN_FILES})

target_link_libraries(IRobotDescriptionMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(IRobotDescriptionMsgs PUBLIC ${IROBDESC_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET IRobotDescriptionMsgs PROPERTY FOLDER "Plugins/Shared")
