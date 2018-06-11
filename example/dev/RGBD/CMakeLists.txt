# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

add_executable(RGBD_test_1a RGBD_test_1a.cpp)
add_executable(RGBD_test_1b_server RGBD_test_1b_server.cpp)
add_executable(RGBD_test_1b_client RGBD_test_1b_client.cpp)

target_link_libraries(RGBD_test_1a ${YARP_LIBRARIES})
target_link_libraries(RGBD_test_1b_server ${YARP_LIBRARIES})
target_link_libraries(RGBD_test_1b_client ${YARP_LIBRARIES})

