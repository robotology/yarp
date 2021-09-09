# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS OR NOT TARGET YARP::YARP_math)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(IMAP2DMSG_THRIFT IMap2DMsgs.thrift)

add_library(IMap2DMsgs OBJECT)

target_sources(IMap2DMsgs PRIVATE ${IMAP2DMSG_THRIFT_GEN_FILES})

target_link_libraries(IMap2DMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_math
)

target_include_directories(IMap2DMsgs PUBLIC ${IMAP2DMSG_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET IMap2DMsgs PROPERTY FOLDER "Devices/Shared")
