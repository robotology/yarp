# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

add_executable(RGBD_test_1a)
target_sources(RGBD_test_1a PRIVATE RGBD_test_1a.cpp)
target_link_libraries(RGBD_test_1a PRIVATE ${YARP_LIBRARIES})

add_executable(RGBD_test_1b_server)
target_sources(RGBD_test_1b_server PRIVATE RGBD_test_1b_server.cpp)
target_link_libraries(RGBD_test_1b_server PRIVATE ${YARP_LIBRARIES})

add_executable(RGBD_test_1b_client)
target_sources(RGBD_test_1b_client PRIVATE RGBD_test_1b_client.cpp)
target_link_libraries(RGBD_test_1b_client PRIVATE ${YARP_LIBRARIES})
