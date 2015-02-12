# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(foobot TYPE foobot INCLUDE "foobot.h")
endif(COMPILE_DEVICE_LIBRARY)

if(NOT SKIP_foobot)
  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  yarp_add_plugin(foobot foobot.cpp foobot.h)
endif(NOT SKIP_foobot)
