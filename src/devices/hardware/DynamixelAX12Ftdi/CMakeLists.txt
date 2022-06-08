# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(dynamixelAX12Ftdi
  CATEGORY device
  TYPE DynamixelAX12FtdiDriver
  INCLUDE "DynamixelAX12FtdiDriver.h"
  EXTRA_CONFIG
    WRAPPER=controlboard
  DEPENDS "YARP_HAS_FTDI"
)

if(NOT SKIP_dynamixelAX12Ftdi)
  yarp_add_plugin(yarp_dynamixelAX12Ftdi)

  target_sources(yarp_dynamixelAX12Ftdi
    PRIVATE
      DynamixelAX12FtdiDriver.cpp
      DynamixelAX12FtdiDriver.h
  )

  target_link_libraries(yarp_dynamixelAX12Ftdi
    PRIVATE
      YARP::YARP_os
      YARP::YARP_dev
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_dev
  )

  target_include_directories(yarp_dynamixelAX12Ftdi SYSTEM PRIVATE ${FTDI_INCLUDE_DIR})
  target_link_libraries(yarp_dynamixelAX12Ftdi PRIVATE ${FTDI_LIBRARY})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS FTDI) (not using targets)

  yarp_install(
    TARGETS yarp_dynamixelAX12Ftdi
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_dynamixelAX12Ftdi PROPERTY FOLDER "Unsupported")
endif()
