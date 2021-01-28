# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

add_executable(moduleLoader)
target_sources(moduleLoader PRIVATE main.cpp)
target_link_libraries(moduleLoader PRIVATE YARP::YARP_os
                                           YARP::YARP_init)
