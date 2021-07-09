# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(yarp_pm_asciiimage)
cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os sig dev REQUIRED)

set(YARP_FORCE_DYNAMIC_PLUGINS TRUE CACHE INTERNAL "yarp_pm_asciiimage is always built with dynamic plugins")
# Warning: the <package> option of yarp_configure_plugins_installation should be different from the plugin name
yarp_configure_plugins_installation(asciiimage_portmonitor)

yarp_prepare_plugin(asciiimage TYPE AsciiImageMonitorObject
                               INCLUDE AsciiImage.h
                               CATEGORY portmonitor
                               INTERNAL
                               QUIET)

yarp_add_plugin(yarp_pm_asciiimage)

target_sources(yarp_pm_asciiimage PRIVATE AsciiImage.cpp
                                          AsciiImage.h)

target_link_libraries(yarp_pm_asciiimage YARP::YARP_os
                                         YARP::YARP_sig
                                         YARP::YARP_dev)

yarp_install(TARGETS yarp_pm_asciiimage
             EXPORT yarp_pm_asciiimage
             COMPONENT runtime
             LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
             ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
             YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
