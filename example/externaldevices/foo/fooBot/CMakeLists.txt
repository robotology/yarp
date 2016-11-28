# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

yarp_prepare_plugin(foobot
                    TYPE yarp::dev::foobot
                    INCLUDE "foobot.h"
                    CATEGORY device
                    DEFAULT ON)

if(NOT SKIP_foobot)
  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  yarp_add_plugin(foobot foobot.cpp foobot.h)
endif(NOT SKIP_foobot)
