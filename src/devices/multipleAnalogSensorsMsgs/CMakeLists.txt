# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(YARP_COMPILE_DEVICE_PLUGINS)
  if(BUILD_SHARED_LIBS AND
     NOT CMAKE_CROSSCOMPILING AND
     NOT ALLOW_IDL_GENERATION AND
     YARP_COMPILE_EXECUTABLES)
    yarp_add_idl(MAS_THRIFT_GEN_FILES multipleAnalogSensorsSerializations.thrift)
    set(MAS_THRIFT_INTERFACE_INCLUDE_DIRS "${YARP_ADD_IDL_INCLUDE_DIR}")
  else()
    yarp_idl_to_dir(INPUT_FILES multipleAnalogSensorsSerializations.thrift
                    OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/idl_generated_code"
                    SOURCES_VAR MAS_THRIFT_GEN_SRCS
                    HEADERS_VAR MAS_THRIFT_GEN_HDRS
                    INCLUDE_DIRS_VAR MAS_THRIFT_INTERFACE_INCLUDE_DIRS)
    set(MAS_THRIFT_GEN_FILES ${MAS_THRIFT_GEN_SRCS} ${MAS_THRIFT_GEN_HDRS})
  endif()

  add_library(multipleAnalogSensorsSerializations OBJECT)

  target_sources(multipleAnalogSensorsSerializations PRIVATE ${MAS_THRIFT_GEN_FILES})

  target_link_libraries(multipleAnalogSensorsSerializations PRIVATE YARP::YARP_os
                                                                    YARP::YARP_sig)
  target_include_directories(multipleAnalogSensorsSerializations PUBLIC ${MAS_THRIFT_INTERFACE_INCLUDE_DIRS})

  set_property(TARGET multipleAnalogSensorsSerializations PROPERTY FOLDER "Libraries/Msgs")
endif()
