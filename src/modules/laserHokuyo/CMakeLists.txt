# Copyright (C) 2010 Marco Randazzo
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

IF (COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(laserHokuyo TYPE laserHokuyo INCLUDE laserHokuyo.h)
  yarp_install(FILES laserHokuyo.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
ENDIF (COMPILE_DEVICE_LIBRARY)

if (NOT SKIP_laserHokuyo)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                       ${YARP_INCLUDE_DIRS})

yarp_add_plugin(laserHokuyo laserHokuyo.h laserHokuyo.cpp)
target_link_libraries(laserHokuyo ${YARP_LIBRARIES})

endif (NOT SKIP_laserHokuyo)
