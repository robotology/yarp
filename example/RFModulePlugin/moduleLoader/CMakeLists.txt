# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_executable(moduleLoader)
target_sources(moduleLoader PRIVATE main.cpp)
target_link_libraries(moduleLoader PRIVATE YARP::YARP_os
                                           YARP::YARP_init)
