# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(YARP_COMPILE_DEVICE_PLUGINS)
  include(YarpChooseIDL)
  yarp_choose_idl(MAS_THRIFT multipleAnalogSensorsSerializations.thrift)

  add_library(multipleAnalogSensorsSerializations OBJECT)

  target_sources(multipleAnalogSensorsSerializations PRIVATE ${MAS_THRIFT_GEN_FILES})

  target_link_libraries(multipleAnalogSensorsSerializations PRIVATE YARP::YARP_os
                                                                    YARP::YARP_sig)
  target_include_directories(multipleAnalogSensorsSerializations PUBLIC ${MAS_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

  set_property(TARGET multipleAnalogSensorsSerializations PROPERTY FOLDER "Libraries/Msgs")
endif()
