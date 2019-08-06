# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
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

  # See https://gitlab.kitware.com/cmake/cmake/issues/14778
  # See https://gitlab.kitware.com/cmake/cmake/merge_requests/1524
  if(NOT ${CMAKE_MINIMUM_REQUIRED_VERSION} VERSION_LESS 3.12)
    message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. object libraries can be used with target_link_libraries now.")
  endif()
  add_library(multipleAnalogSensorsSerializations OBJECT ${MAS_THRIFT_GEN_FILES})
  target_include_directories(multipleAnalogSensorsSerializations PRIVATE $<TARGET_PROPERTY:YARP::YARP_os,INTERFACE_INCLUDE_DIRECTORIES>)
  target_compile_definitions(multipleAnalogSensorsSerializations PRIVATE $<TARGET_PROPERTY:YARP::YARP_os,INTERFACE_COMPILE_DEFINITIONS>)
  target_include_directories(multipleAnalogSensorsSerializations PRIVATE $<TARGET_PROPERTY:YARP::YARP_sig,INTERFACE_INCLUDE_DIRECTORIES>)
  target_compile_definitions(multipleAnalogSensorsSerializations PRIVATE $<TARGET_PROPERTY:YARP::YARP_sig,INTERFACE_COMPILE_DEFINITIONS>)
  target_include_directories(multipleAnalogSensorsSerializations PUBLIC ${MAS_THRIFT_INTERFACE_INCLUDE_DIRS})

  set_property(TARGET multipleAnalogSensorsSerializations PROPERTY FOLDER "Libraries/Msgs")
endif()
