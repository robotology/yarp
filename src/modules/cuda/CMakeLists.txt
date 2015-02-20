# Copyright: (C) 2009 RobotCub Consortium
# Authors: Giacomo Spigler, Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(cuda TYPE CUDAGPU INCLUDE "CUDADeviceDriver.h")
  yarp_install(FILES cuda.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif(COMPILE_DEVICE_LIBRARY)

if(NOT SKIP_cuda)
#  set(CUDA_NUMTHREADS "64" CACHE STRING "lemieux" FORCE)
  set(CUDA_NUMTHREADS 64 CACHE STRING "")
  set(CUDA_NUMTHREADBLOCKS 256 CACHE STRING "")

  find_package(YARP REQUIRED)

  find_library(lib_cudart cudart)
  find_library(lib_cuda cuda)
  find_library(lib_cutil cutil)
  find_library(lib_GL GL)

  if(lib_cudart AND lib_cuda AND lib_cutil AND lib_GL)

    add_definitions(-DCUDA_NUMTHREADS=${CUDA_NUMTHREADS} -DCUDA_NUMTHREADBLOCKS=${CUDA_NUMTHREADBLOCKS})
    yarp_add_plugin(ycuda CUDADeviceDriver.cpp CUDADeviceDriver.h FunctionClass.h)
    target_link_libraries(ycuda ${lib_cudart} ${lib_cuda} ${lib_GL} ${lib_cutil})
    yarp_install(TARGETS ycuda
                 EXPORT YARP
                 COMPONENT runtime
                 LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

  else()
    message(FATAL_ERROR "could not find all libraries needed by cuda driver")
  endif()

endif()

