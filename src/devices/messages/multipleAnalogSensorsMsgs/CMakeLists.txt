# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(MAS_THRIFT multipleAnalogSensorsSerializations.thrift)

add_library(multipleAnalogSensorsSerializations OBJECT)

target_sources(multipleAnalogSensorsSerializations PRIVATE ${MAS_THRIFT_GEN_FILES})

target_link_libraries(multipleAnalogSensorsSerializations
  PRIVATE
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(multipleAnalogSensorsSerializations PUBLIC ${MAS_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET multipleAnalogSensorsSerializations PROPERTY FOLDER "Devices/Shared")
