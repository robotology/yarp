# Copyright (C) 2015 Marco Randazzo
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

IF (COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(rpLidar TYPE RpLidar INCLUDE rpLidar.h)
  yarp_install(FILES rpLidar.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
ENDIF (COMPILE_DEVICE_LIBRARY)

if (NOT SKIP_rpLidar)

  include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                      ${YARP_INCLUDE_DIRS})

  yarp_add_plugin(rpLidar rpLidar.h rpLidar.cpp)
  target_link_libraries(rpLidar ${YARP_LIBRARIES})
  yarp_install(TARGETS rpLidar
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

endif (NOT SKIP_rpLidar)
