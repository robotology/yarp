# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os dev REQUIRED)

yarp_prepare_plugin(foobot
                    TYPE foobot
                    INCLUDE "foobot.h"
                    CATEGORY device
                    DEFAULT ON)

if(NOT SKIP_foobot)
  yarp_add_plugin(foobot)

  target_sources(foobot PRIVATE foobot.cpp
                                foobot.h)
  target_link_libraries(foobot PRIVATE YARP::YARP_os
                                       YARP::YARP_dev)
endif(NOT SKIP_foobot)
