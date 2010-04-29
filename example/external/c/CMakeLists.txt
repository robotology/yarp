CMAKE_MINIMUM_REQUIRED(VERSION 2.0)

PROJECT(yarp_without_yarp_in_c)

ADD_EXECUTABLE(yarpread yarpread.c yarpmin.c yarpmin.h)

ADD_EXECUTABLE(yarprpc yarprpc.c yarpmin.c yarpmin.h)

ADD_EXECUTABLE(yarpreadimage yarpreadimage.c yarpmin.c yarpmin.h)
