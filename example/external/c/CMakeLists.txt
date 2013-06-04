# Copyright: (C) 2010 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.7)

PROJECT(yarp_without_yarp_in_c)

ADD_EXECUTABLE(yarpread yarpread.c yarpmin.c yarpmin.h)

ADD_EXECUTABLE(yarprpc yarprpc.c yarpmin.c yarpmin.h)

ADD_EXECUTABLE(yarpreadimage yarpreadimage.c yarpmin.c yarpmin.h)

ADD_EXECUTABLE(yarpreadsound yarpreadsound.c yarpmin.c yarpmin.h)
