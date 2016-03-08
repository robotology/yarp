# Copyright (C) 2016 iCub Facility
# Authors: Alberto Cardellino
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})

include_directories(${YARP_INCLUDE_DIRS})
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

include(YarpPlugin)
include(YarpInstallationHelpers)

set(YARP_FORCE_DYNAMIC_PLUGINS TRUE CACHE INTERNAL "yarp_pm_depthimage is always built with dynamic plugins")
yarp_configure_external_installation(yarp)

yarp_prepare_plugin(depthimage TYPE DepthImageConverter
                               INCLUDE DepthImage.h
                               CATEGORY portmonitor)
yarp_install(FILES depthimage.ini
             COMPONENT runtime
             DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

yarp_add_plugin(yarp_pm_depthimage
                DepthImage.cpp
                DepthImage.h)
target_link_libraries(yarp_pm_depthimage ${YARP_LIBRARIES})
yarp_install(TARGETS yarp_pm_depthimage
             EXPORT YARP
             COMPONENT runtime
             LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
             ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
