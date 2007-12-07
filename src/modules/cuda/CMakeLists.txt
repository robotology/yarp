
FIND_PACKAGE(YARP REQUIRED)

FIND_LIBRARY(lib_cudart cudart)
FIND_LIBRARY(lib_pthread cuda)
FIND_LIBRARY(lib_GL cutil)
FIND_LIBRARY(lib_glut glut)

SET (OK FALSE)
IF(lib_cudart AND lib_cuda AND lib_cutil AND lib_GL)
  SET (OK TRUE)
ENDIF(lib_cudart AND lib_cuda AND lib_cutil AND lib_GL)

IF (OK)

  LINK_LIBRARIES(${lib_cudart} ${lib_cuda} ${lib_cutil} ${lib_GL})

  IF (COMPILE_DEVICE_LIBRARY)
    PREPARE_DEVICE(cuda TYPE CUDAGPU INCLUDE "CUDADeviceDriver.h")
  ENDIF (COMPILE_DEVICE_LIBRARY)

  ADD_LIBRARY(ycuda CUDADeviceDriver.cpp CUDADeviceDriver.h FunctionClass.h)

ELSE (OK)

  MESSAGE(STATUS "could not find all libraries needed by cuda driver")

ENDIF (OK)

