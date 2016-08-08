# Copyright (C) 2016 iCub Facility
# Authors: Alberto Cardellino
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_plugin(depthimage_portmonitor TYPE DepthImageConverter
                                             INCLUDE DepthImage.h
                                             CATEGORY portmonitor)
  yarp_install(FILES depthimage.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif(COMPILE_PLUGIN_LIBRARY)

if(NOT SKIP_depthimage_portmonitor)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)

  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS})
  yarp_add_plugin(yarp_pm_depthimage
                  DepthImage.cpp
                  DepthImage.h)
  target_link_libraries(yarp_pm_depthimage YARP_OS YARP_sig)
  yarp_install(TARGETS yarp_pm_depthimage
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
endif()



