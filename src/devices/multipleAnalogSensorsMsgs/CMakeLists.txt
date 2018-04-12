# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(CREATE_DEVICE_LIBRARY_MODULES)
  yarp_add_idl(MAS_THRIFT_GEN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/multipleAnalogSensorsSerializations.thrift")

  # See https://gitlab.kitware.com/cmake/cmake/issues/14778
  # See https://gitlab.kitware.com/cmake/cmake/merge_requests/1524
  if(NOT ${CMAKE_MINIMUM_REQUIRED_VERSION} VERSION_LESS 3.12)
    message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. object libraries can be used with target_link_libraries now.")
  endif()
  add_library(multipleAnalogSensorsSerializations OBJECT ${MAS_THRIFT_GEN_FILES})
  target_include_directories(multipleAnalogSensorsSerializations PRIVATE $<TARGET_PROPERTY:YARP::YARP_OS,INTERFACE_INCLUDE_DIRECTORIES>)
  target_compile_definitions(multipleAnalogSensorsSerializations PRIVATE $<TARGET_PROPERTY:YARP::YARP_OS,INTERFACE_COMPILE_DEFINITIONS>)
  target_include_directories(multipleAnalogSensorsSerializations PRIVATE $<TARGET_PROPERTY:YARP::YARP_sig,INTERFACE_INCLUDE_DIRECTORIES>)
  target_compile_definitions(multipleAnalogSensorsSerializations PRIVATE $<TARGET_PROPERTY:YARP::YARP_sig,INTERFACE_COMPILE_DEFINITIONS>)
  target_include_directories(multipleAnalogSensorsSerializations INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/include)

  set_property(TARGET multipleAnalogSensorsSerializations PROPERTY FOLDER "Libraries/Msgs")
endif()
