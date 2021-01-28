# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

PROJECT(yarp_without_yarp_in_c)

ADD_EXECUTABLE(yarpread yarpread.c yarpmin.c yarpmin.h)

ADD_EXECUTABLE(yarprpc yarprpc.c yarpmin.c yarpmin.h)

ADD_EXECUTABLE(yarpreadimage yarpreadimage.c yarpmin.c yarpmin.h)

ADD_EXECUTABLE(yarpreadsound yarpreadsound.c yarpmin.c yarpmin.h)
