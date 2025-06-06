# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(RGBDSENSOR_THRIFT RGBDSensorMsgs.thrift)

add_library(RGBDSensorMsgs OBJECT)

target_sources(RGBDSensorMsgs PRIVATE ${RGBDSENSOR_THRIFT_GEN_FILES})

target_link_libraries(RGBDSensorMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(RGBDSensorMsgs PUBLIC ${RGBDSENSOR_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET RGBDSensorMsgs PROPERTY FOLDER "Plugins/Shared")
