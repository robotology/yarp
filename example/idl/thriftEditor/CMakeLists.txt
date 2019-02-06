# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.5)

find_package(YARP REQUIRED)

yarp_idl_to_dir(settings.thrift ${CMAKE_BINARY_DIR}/settings SRC HDR INCLUDES)

include_directories(${INCLUDES})
add_executable(main main.cpp ${SRC} ${HDR})
target_link_libraries(main ${YARP_LIBRARIES})
