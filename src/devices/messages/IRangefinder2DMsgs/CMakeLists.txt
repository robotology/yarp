# SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(IRANGEFINDER2DMSG_THRIFT IRangefinder2DMsgs.thrift)

add_library(IRangefinder2DMsgs OBJECT)

target_sources(IRangefinder2DMsgs PRIVATE ${IRANGEFINDER2DMSG_THRIFT_GEN_FILES})

target_link_libraries(IRangefinder2DMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(IRangefinder2DMsgs PUBLIC ${IRANGEFINDER2DMSG_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET IRangefinder2DMsgs PROPERTY FOLDER "Plugins/Shared")
