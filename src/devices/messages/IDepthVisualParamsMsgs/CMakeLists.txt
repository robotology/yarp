# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(IDEPTHVISUALPARAMS_THRIFT IDepthVisualParamsMsgs.thrift)

add_library(IDepthVisualParamsMsgs OBJECT)

target_sources(IDepthVisualParamsMsgs PRIVATE ${IDEPTHVISUALPARAMS_THRIFT_GEN_FILES})

target_link_libraries(IDepthVisualParamsMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(IDepthVisualParamsMsgs PUBLIC ${IDEPTHVISUALPARAMS_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET IDepthVisualParamsMsgs PROPERTY FOLDER "Plugins/Shared")
