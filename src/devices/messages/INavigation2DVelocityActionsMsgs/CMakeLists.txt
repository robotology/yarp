# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS OR NOT TARGET YARP::YARP_math)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(FTM_THRIFT INavigation2DVelocityActionsMsgs.thrift)

add_library(INavigation2DVelocityActionsMsgs OBJECT)

target_sources(INavigation2DVelocityActionsMsgs PRIVATE ${FTM_THRIFT_GEN_FILES})

target_link_libraries(INavigation2DVelocityActionsMsgs
  PRIVATE
    YARP::YARP_os
    YARP::YARP_dev
    YARP::YARP_sig
    YARP::YARP_math
)

target_include_directories(INavigation2DVelocityActionsMsgs PUBLIC ${FTM_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET INavigation2DVelocityActionsMsgs PROPERTY FOLDER "Plugins/Shared")
