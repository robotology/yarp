# Copyright (C) 2010 Ze Ji
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
  YARP_PREPARE_DEVICE(dynamixelAX12Ftdi TYPE DynamixelAX12FtdiDriver INCLUDE "DynamixelAX12FtdiDriver.h" WRAPPER controlboard)
  YARP_ADD_DEVICE_FINGERPRINT(dynamixelAX12Ftdi.ini dynamixelAX12Ftdi)
endif(COMPILE_DEVICE_LIBRARY)

if(NOT SKIP_dynamixelAX12Ftdi)
  find_package(YARP REQUIRED)
  set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_MODULE_PATH})
  find_package(FTDI REQUIRED)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${FTDI_INCLUDE_DIR})
  yarp_add_plugin(dynamixelAX12FtdiDriver DynamixelAX12FtdiDriver.cpp DynamixelAX12FtdiDriver.h)
  target_link_libraries(dynamixelAX12FtdiDriver ${YARP_LIBRARIES} ${FTDI_LIBRARY})
endif(NOT SKIP_dynamixelAX12Ftdi)


