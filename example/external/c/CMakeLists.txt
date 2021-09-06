# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)

PROJECT(yarp_without_yarp_in_c)

ADD_EXECUTABLE(yarpread yarpread.c yarpmin.c yarpmin.h)

ADD_EXECUTABLE(yarprpc yarprpc.c yarpmin.c yarpmin.h)

ADD_EXECUTABLE(yarpreadimage yarpreadimage.c yarpmin.c yarpmin.h)

ADD_EXECUTABLE(yarpreadsound yarpreadsound.c yarpmin.c yarpmin.h)
