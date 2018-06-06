# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(zfp TYPE ZfpMonitorObject
                        INCLUDE zfpPortmonitor.h
                        CATEGORY portmonitor
                        DEPENDS "ENABLE_yarpcar_portmonitor;YARP_HAS_ZFP")

if(NOT SKIP_zfp)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  include_directories(SYSTEM ${ZFP_INCLUDE_DIRS})

  yarp_add_plugin(yarp_pm_zfp zfpPortmonitor.cpp
                              zfpPortmonitor.h)
  target_link_libraries(yarp_pm_zfp YARP::YARP_OS
                                    YARP::YARP_sig
                                    ${ZFP_LIBRARIES})

  yarp_install(TARGETS yarp_pm_zfp
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES zfp.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_pm_zfp PROPERTY FOLDER "Plugins/Port Monitor")
endif()
