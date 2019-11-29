# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

find_package(YARP REQUIRED)

yarp_prepare_plugin(foodevice
                    TYPE yarp::dev::foodevice
                    INCLUDE "foodevice.h"
                    CATEGORY device
                    DEFAULT ON)

if(NOT SKIP_foodevice)
  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
  yarp_add_plugin(foodevice foodevice.cpp foodevice.h)
endif(NOT SKIP_foodevice)
