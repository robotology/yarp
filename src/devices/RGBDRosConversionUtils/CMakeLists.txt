# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

add_library(RGBDRosConversionUtils OBJECT)

target_sources(RGBDRosConversionUtils PRIVATE RGBDRosConversionUtils.cpp
                                              RGBDRosConversionUtils.h
                                              rosPixelCode.h
                                              rosPixelCode.cpp)

target_include_directories(RGBDRosConversionUtils PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(RGBDRosConversionUtils PRIVATE YARP::YARP_os
                                                     YARP::YARP_sig
                                                     YARP::YARP_rosmsg
                                                     YARP::YARP_dev)

set_property(TARGET RGBDRosConversionUtils PROPERTY FOLDER "Libraries/Msgs")
