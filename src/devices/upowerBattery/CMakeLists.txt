# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

find_package(Qt5 COMPONENTS DBus QUIET)

yarp_prepare_plugin(upowerBattery
                    CATEGORY device
                    TYPE UpowerBattery
                    INCLUDE upowerBattery.h
                    EXTRA_CONFIG WRAPPER=batteryWrapper
                    DEPENDS "UNIX; NOT APPLE; YARP_HAS_Qt5; Qt5DBus_FOUND")

if(NOT SKIP_upowerBattery)
  yarp_add_plugin(yarp_upowerBattery)

  target_sources(yarp_upowerBattery PRIVATE upowerBattery.cpp
                                            upowerBattery.h)

  target_link_libraries(yarp_upowerBattery PRIVATE YARP::YARP_os
                                                   YARP::YARP_sig
                                                   YARP::YARP_dev
                                                   Qt5::DBus)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      "Qt5 COMPONENTS DBus")

  yarp_install(TARGETS yarp_upowerBattery
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_upowerBattery PROPERTY FOLDER "Plugins/Device")
endif()
