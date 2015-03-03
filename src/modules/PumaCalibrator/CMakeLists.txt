# Copyright: (C) 2009 RobotCub Consortium
# Authors: Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
IF (COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(pumacalibrator TYPE yarp::dev::PumaCalibrator INCLUDE PumaCalibrator.h )
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_pumacalibrator)

  FIND_PACKAGE(YARP REQUIRED)
#	SET(YARP_DEPENDENCIES "medvc60f.lib")
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

  yarp_add_plugin(pumacalibrator PumaCalibrator.cpp  PumaCalibrator.h )
  yarp_install(TARGETS pumacalibrator
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

ENDIF (NOT SKIP_pumacalibrator)

