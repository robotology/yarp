# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(cuda
                    CATEGORY device
                    TYPE CUDAGPU
                    INCLUDE "CUDADeviceDriver.h"
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_HAS_CUDA")

if(NOT SKIP_cuda)
  set(CUDA_NUMTHREADS 64 CACHE STRING "")
  set(CUDA_NUMTHREADBLOCKS 256 CACHE STRING "")

  add_definitions(-DCUDA_NUMTHREADS=${CUDA_NUMTHREADS}
                  -DCUDA_NUMTHREADBLOCKS=${CUDA_NUMTHREADBLOCKS})

  yarp_add_plugin(yarp_cuda CUDADeviceDriver.cpp
                            CUDADeviceDriver.h
                            FunctionClass.h)
  target_link_libraries(yarp_cuda YARP::YARP_OS
                                  YARP::YARP_sig
                                  YARP::YARP_dev
                                  ${CUDA_LIBRARIES})
  yarp_install(TARGETS yarp_cuda
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES cuda.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_cuda PROPERTY FOLDER "Plugins/Device")
endif()
