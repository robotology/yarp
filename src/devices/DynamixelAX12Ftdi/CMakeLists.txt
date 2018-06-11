# Copyright (C) 2010 Ze Ji
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE

yarp_prepare_plugin(dynamixelAX12Ftdi
                    CATEGORY device
                    TYPE yarp::dev::DynamixelAX12FtdiDriver
                    INCLUDE "DynamixelAX12FtdiDriver.h"
                    EXTRA_CONFIG WRAPPER=controlboard
                    DEPENDS "YARP_HAS_FTDI")

if(NOT SKIP_dynamixelAX12Ftdi)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  include_directories(SYSTEM ${FTDI_INCLUDE_DIR})
  yarp_add_plugin(yarp_dynamixelAX12Ftdi DynamixelAX12FtdiDriver.cpp
                                         DynamixelAX12FtdiDriver.h)
  target_link_libraries(yarp_dynamixelAX12Ftdi YARP::YARP_OS
                                               YARP::YARP_dev
                                               ${FTDI_LIBRARY})
  yarp_install(TARGETS yarp_dynamixelAX12Ftdi
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES dynamixelAX12Ftdi.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_dynamixelAX12Ftdi PROPERTY FOLDER "Unsupported")
endif()


