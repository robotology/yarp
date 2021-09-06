# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os dev REQUIRED)

yarp_prepare_plugin(bazbot
  CATEGORY device
  TYPE bazbot
  INCLUDE "bazbot.h"
  DEFAULT ON
)

if(NOT SKIP_bazbot)
  yarp_add_plugin(bazbot)

  target_sources(bazbot
    PRIVATE
      bazbot.cpp
      bazbot.h
  )
  target_link_libraries(bazbot
    PRIVATE
      YARP::YARP_os
      YARP::YARP_dev
  )
endif(NOT SKIP_bazbot)
