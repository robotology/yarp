# Copyright (C) 2016 Marco Randazzo
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

IF (COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(transformClient TYPE TransformClient INCLUDE transformClient.h)
  yarp_install(FILES transformClient.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
ENDIF (COMPILE_DEVICE_LIBRARY)

if (NOT SKIP_transformClient)

  include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                      ${YARP_INCLUDE_DIRS})

  yarp_add_plugin(transformClient transformClient.h transformClient.cpp)
  target_link_libraries(transformClient ${YARP_LIBRARIES})
  yarp_install(TARGETS transformClient
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

endif (NOT SKIP_transformClient)
