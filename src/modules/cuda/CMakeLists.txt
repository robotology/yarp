# Copyright: (C) 2009 RobotCub Consortium
# Authors: Giacomo Spigler, Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
IF (COMPILE_DEVICE_LIBRARY)
  YARP_PREPARE_DEVICE(cuda TYPE CUDAGPU INCLUDE "CUDADeviceDriver.h")
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_cuda)
#  SET( CUDA_NUMTHREADS "64" CACHE STRING "lemieux" FORCE )
  SET( CUDA_NUMTHREADS 64 CACHE STRING "" )
  SET( CUDA_NUMTHREADBLOCKS 256 CACHE STRING "" )



  FIND_PACKAGE(YARP REQUIRED)

  FIND_LIBRARY(lib_cudart cudart)
  FIND_LIBRARY(lib_cuda cuda)
  FIND_LIBRARY(lib_cutil cutil)
  FIND_LIBRARY(lib_GL GL)


  SET (OK FALSE)
  IF(lib_cudart AND lib_cuda AND lib_cutil AND lib_GL)
    SET (OK TRUE)
  ENDIF(lib_cudart AND lib_cuda AND lib_cutil AND lib_GL)

  IF (OK)

    LINK_LIBRARIES(${lib_cudart} ${lib_cuda} ${lib_GL} ${lib_cutil})

    ADD_DEFINITIONS(-DCUDA_NUMTHREADS=${CUDA_NUMTHREADS} -DCUDA_NUMTHREADBLOCKS=${CUDA_NUMTHREADBLOCKS})

    ADD_LIBRARY(ycuda CUDADeviceDriver.cpp CUDADeviceDriver.h FunctionClass.h)

  ELSE (OK)

    MESSAGE(STATUS "could not find all libraries needed by cuda driver")

  ENDIF (OK)

ENDIF (NOT SKIP_cuda)

