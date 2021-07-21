# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause


yarp_prepare_plugin(microphoneFilter TYPE MicrophoneFilter
                                        INCLUDE MicrophoneFilter.h
                                        CATEGORY portmonitor
                                        DEPENDS "ENABLE_yarpcar_portmonitor")

if(SKIP_microphoneFilter)
  return()
endif()

yarp_add_plugin(yarp_pm_MicrophoneFilter)

target_sources(yarp_pm_MicrophoneFilter PRIVATE MicrophoneFilter.cpp
                                      MicrophoneFilter.h)

target_link_libraries(yarp_pm_MicrophoneFilter YARP::YARP_os
                                     YARP::YARP_sig
                                     YARP::YARP_dev)

yarp_install(TARGETS yarp_pm_MicrophoneFilter
             EXPORT yarp_pm_MicrophoneFilter
             COMPONENT runtime
             LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
             ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
             YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

set_property(TARGET yarp_pm_MicrophoneFilter PROPERTY FOLDER "Plugins/Port Monitor")
