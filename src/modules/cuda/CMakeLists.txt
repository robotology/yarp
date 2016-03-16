# Copyright: (C) 2009 RobotCub Consortium
# Authors: Giacomo Spigler, Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(cuda
                      TYPE CUDAGPU
                      INCLUDE "CUDADeviceDriver.h")
  yarp_install(FILES cuda.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()

if(NOT SKIP_cuda)
  set(CUDA_NUMTHREADS 64 CACHE STRING "")
  set(CUDA_NUMTHREADBLOCKS 256 CACHE STRING "")

  add_definitions(-DCUDA_NUMTHREADS=${CUDA_NUMTHREADS}
                  -DCUDA_NUMTHREADBLOCKS=${CUDA_NUMTHREADBLOCKS})

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})

  yarp_add_plugin(yarp_cuda CUDADeviceDriver.cpp
                            CUDADeviceDriver.h
                            FunctionClass.h)
  target_link_libraries(yarp_cuda YARP_OS
                                  YARP_sig
                                  YARP_dev
                                  ${CUDA_LIBRARIES})
  yarp_install(TARGETS yarp_cuda
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
endif()
