# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.5)
project(yayarpview)

find_package(YARP COMPONENTS OS sig REQUIRED)

find_package(X11)

set(yayarpview_HDRS CImg.h
                    stmac.h
                    YarpCImgViewer.h)

set(yayarpview_SRCS main.cpp
                    YarpCImgViewer.cpp)

add_executable(yayarpview ${yayarpview_HDRS}
                          ${yayarpview_SRCS})

target_include_directories(yayarpview SYSTEM PRIVATE ${X11_INCLUDE_DIR})

target_link_libraries(yayarpview PRIVATE ${YARP_LIBRARIES}
                                         ${X11_LIBRARIES}
                                         pthread)
