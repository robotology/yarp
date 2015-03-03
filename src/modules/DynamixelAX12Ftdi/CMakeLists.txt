# Copyright (C) 2010 Ze Ji
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(dynamixelAX12Ftdi TYPE yarp::dev::DynamixelAX12FtdiDriver INCLUDE "DynamixelAX12FtdiDriver.h" WRAPPER controlboard)
  yarp_install(FILES dynamixelAX12Ftdi.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif(COMPILE_DEVICE_LIBRARY)

if(NOT SKIP_dynamixelAX12Ftdi)
  find_package(YARP REQUIRED)
  find_package(FTDI REQUIRED)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${FTDI_INCLUDE_DIR})
  include_directories(${YARP_INCLUDE_DIRS})
  yarp_add_plugin(dynamixelAX12FtdiDriver DynamixelAX12FtdiDriver.cpp DynamixelAX12FtdiDriver.h)
  target_link_libraries(dynamixelAX12FtdiDriver ${YARP_LIBRARIES} ${FTDI_LIBRARY})
  yarp_install(TARGETS dynamixelAX12FtdiDriver
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
endif(NOT SKIP_dynamixelAX12Ftdi)


