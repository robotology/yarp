# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os dev REQUIRED)

yarp_prepare_plugin(bazdevice
                    CATEGORY device
                    TYPE bazdevice
                    INCLUDE "bazdevice.h"
                    DEFAULT ON)

if(NOT SKIP_bazdevice)
  yarp_add_plugin(bazdevice)

  target_sources(bazdevice PRIVATE bazdevice.cpp
                                   bazdevice.h)
  target_link_libraries(bazdevice PRIVATE YARP::YARP_os
                                          YARP::YARP_dev)
endif(NOT SKIP_bazdevice)
