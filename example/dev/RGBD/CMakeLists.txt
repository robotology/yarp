# Copyright: (C) 2018  Istituto Italiano di Tecnologia (IIT) - iCub Facility
# Authors: Alberto Cardellino  <alberto.cardellino@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


add_executable(RGBD_test_1a RGBD_test_1a.cpp)
add_executable(RGBD_test_1b_server RGBD_test_1b_server.cpp)
add_executable(RGBD_test_1b_client RGBD_test_1b_client.cpp)

target_link_libraries(RGBD_test_1a ${YARP_LIBRARIES})
target_link_libraries(RGBD_test_1b_server ${YARP_LIBRARIES})
target_link_libraries(RGBD_test_1b_client ${YARP_LIBRARIES})

