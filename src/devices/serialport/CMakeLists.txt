# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(serialport
                    CATEGORY device
                    TYPE SerialDeviceDriver
                    INCLUDE SerialDeviceDriver.h
                    EXTRA_CONFIG WRAPPER=serial
                    DEPENDS "YARP_HAS_ACE")

if(NOT SKIP_serialport)
  yarp_add_plugin(yarp_serialport)

  target_sources(yarp_serialport PRIVATE SerialDeviceDriver.cpp
                                         SerialDeviceDriver.h)

  target_link_libraries(yarp_serialport PRIVATE YARP::YARP_os
                                                YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_dev)

  target_compile_definitions(yarp_serialport PRIVATE YARP_HAS_ACE)
  target_link_libraries(yarp_serialport PRIVATE ACE::ACE)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ACE)

  yarp_install(TARGETS yarp_serialport
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_serialport PROPERTY FOLDER "Plugins/Device")
endif()
