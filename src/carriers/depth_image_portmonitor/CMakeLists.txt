# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(depthimage TYPE DepthImageConverter
                               INCLUDE DepthImage.h
                               CATEGORY portmonitor
                               DEPENDS "CREATE_OPTIONAL_CARRIERS;ENABLE_yarpcar_portmonitor")

if(NOT SKIP_depthimage)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  yarp_add_plugin(yarp_pm_depthimage
                  DepthImage.cpp
                  DepthImage.h)
  target_link_libraries(yarp_pm_depthimage YARP::YARP_OS
                                           YARP::YARP_sig)
  yarp_install(TARGETS yarp_pm_depthimage
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES depthimage.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_pm_depthimage PROPERTY FOLDER "Plugins/Port Monitor")
endif()
