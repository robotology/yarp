# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(fakeBattery
                    CATEGORY device
                    TYPE FakeBattery
                    INCLUDE fakeBattery.h
                    EXTRA_CONFIG WRAPPER=batteryWrapper)

if(NOT SKIP_fakeBattery)
  yarp_add_plugin(yarp_fakeBattery)

  include(YarpChooseIDL)
  yarp_choose_idl(IDL FakeBatteryService.thrift)

  target_sources(yarp_fakeBattery PRIVATE fakeBattery.cpp
                                          fakeBattery.h
                                          ${IDL_GEN_FILES})

  target_include_directories(yarp_fakeBattery PRIVATE ${IDL_BUILD_INTERFACE_INCLUDE_DIRS})

  target_link_libraries(yarp_fakeBattery PRIVATE YARP::YARP_os
                                                 YARP::YARP_sig
                                                 YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  yarp_install(TARGETS yarp_fakeBattery
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeBattery PROPERTY FOLDER "Plugins/Device/Fake")
endif()
