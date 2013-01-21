# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

IF (COMPILE_DEVICE_LIBRARY)
  YARP_PREPARE_DEVICE(nvidia TYPE NVIDIAGPU INCLUDE "NVIDIADeviceDriver.h")
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_nvidia)

 FIND_PACKAGE(YARP REQUIRED)

 FIND_LIBRARY(lib_CgGL CgGL)
 FIND_LIBRARY(lib_pthread pthread)
 FIND_LIBRARY(lib_GL GL)
 FIND_LIBRARY(lib_glut glut)

 SET (OK FALSE)

 IF(lib_CgGL AND lib_pthread AND lib_GL AND lib_glut)
  SET (OK TRUE)
 ENDIF(lib_CgGL AND lib_pthread AND lib_GL AND lib_glut)

 IF (NOT WINDOWS)
  FIND_LIBRARY(lib_GLEW GLEW)
  IF (NOT lib_GLEW)
    SET (OK FALSE)
  ENDIF (NOT lib_GLEW)
 ENDIF (NOT WINDOWS)


 IF (OK)

  ADD_DEFINITIONS(-DGL_GLEXT_PROTOTYPES -DGLX_GLXEXT_PROTOTYPES)

  LINK_LIBRARIES(pthread GL glut CgGL)
  IF (NOT WINDOWS)
    LINK_LIBRARIES(GLEW)
  ENDIF (NOT WINDOWS)	

  ADD_LIBRARY(nvidia NVIDIADeviceDriver.cpp NVIDIADeviceDriver.h FBO_Filter.h)

 ELSE (OK)

  MESSAGE(STATUS "could not find all libraries needed by nvidia driver")

 ENDIF (OK)

ENDIF (NOT SKIP_nvidia)
