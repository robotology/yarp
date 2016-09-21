# Copyright (C) 2010 Ze Ji
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(dynamixelAX12Ftdi
                    CATEGORY device
                    TYPE yarp::dev::DynamixelAX12FtdiDriver
                    INCLUDE "DynamixelAX12FtdiDriver.h"
                    EXTRA_CONFIG WRAPPER=controlboard
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_HAS_FTDI")

if(NOT SKIP_dynamixelAX12Ftdi)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})
  include_directories(SYSTEM ${FTDI_INCLUDE_DIR})
  yarp_add_plugin(yarp_dynamixelAX12Ftdi DynamixelAX12FtdiDriver.cpp
                                         DynamixelAX12FtdiDriver.h)
  target_link_libraries(yarp_dynamixelAX12Ftdi YARP_OS
                                               YARP_dev
                                               ${FTDI_LIBRARY})
  yarp_install(TARGETS yarp_dynamixelAX12Ftdi
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES dynamixelAX12Ftdi.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()


