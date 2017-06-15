# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)

find_package(YARP REQUIRED)

yarp_prepare_plugin(bazbot
                    CATEGORY device
                    TYPE yarp::dev::bazbot
                    INCLUDE "bazbot.h"
                    DEFAULT ON)

if(NOT SKIP_bazbot)
  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
  yarp_add_plugin(bazbot bazbot.cpp bazbot.h)
endif(NOT SKIP_bazbot)
