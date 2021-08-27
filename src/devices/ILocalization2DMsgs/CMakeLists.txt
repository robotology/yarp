# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS OR NOT TARGET YARP::YARP_math)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(ILOC2D_THRIFT ILocalization2DMsgs.thrift)

add_library(ILocalization2DMsgs OBJECT)

target_sources(ILocalization2DMsgs PRIVATE ${ILOC2D_THRIFT_GEN_FILES})

target_link_libraries(ILocalization2DMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_math
)

target_include_directories(ILocalization2DMsgs PUBLIC ${ILOC2D_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET ILocalization2DMsgs PROPERTY FOLDER "Devices/Shared")
