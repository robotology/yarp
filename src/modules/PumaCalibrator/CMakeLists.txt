
IF (COMPILE_DEVICE_LIBRARY)
  PREPARE_DEVICE(pumacalibrator TYPE PumaCalibrator INCLUDE PumaCalibrator.h )
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_pumacalibrator)

  FIND_PACKAGE(YARP REQUIRED)
#	SET(YARP_DEPENDENCIES "medvc60f.lib")
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

  ADD_LIBRARY(pumacalibrator PumaCalibrator.cpp  PumaCalibrator.h )

ENDIF (NOT SKIP_pumacalibrator)

