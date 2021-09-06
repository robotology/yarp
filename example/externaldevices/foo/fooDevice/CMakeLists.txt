# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os dev REQUIRED)

yarp_prepare_plugin(foodevice
  TYPE foodevice
  INCLUDE "foodevice.h"
  CATEGORY device
  DEFAULT ON
)

if(NOT SKIP_foodevice)
  yarp_add_plugin(foodevice)

  target_sources(foodevice
    PRIVATE
      foodevice.cpp
      foodevice.h
  )
  target_link_libraries(foodevice
    PRIVATE
      YARP::YARP_os
      YARP::YARP_dev
  )
endif(NOT SKIP_foodevice)
