# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(NOT TARGET YARP::YARP_math)
    return()
endif()

if(YARP_COMPILE_DEVICE_PLUGINS)
  include(YarpChooseIDL)
  yarp_choose_idl(FTM_THRIFT frameTransformStorageMsgs.thrift)

  add_library(frameTransformStorageMsgs OBJECT)

  target_sources(frameTransformStorageMsgs PRIVATE ${FTM_THRIFT_GEN_FILES})

  target_link_libraries(frameTransformStorageMsgs PRIVATE YARP::YARP_os
                                                   YARP::YARP_sig
                                                   YARP::YARP_math)
  target_include_directories(frameTransformStorageMsgs PUBLIC ${FTM_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

  set_property(TARGET frameTransformStorageMsgs PROPERTY FOLDER "Libraries/Msgs")
endif()
