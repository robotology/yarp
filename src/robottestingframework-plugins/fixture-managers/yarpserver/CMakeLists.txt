# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(robottestingframework_fixturemanager_yarpserver)

add_definitions(-DSHLIBPP_FILTER_API)

robottestingframework_add_plugin(robottestingframework_fixturemanager_yarpserver
  OUTPUT_NAME yarpserver
  SOURCES yarpserverfixture.cpp
)

target_link_libraries(robottestingframework_fixturemanager_yarpserver
  PRIVATE
    RobotTestingFramework::RTF
    RobotTestingFramework::RTF_dll
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_serversql
)

yarp_install(
  TARGETS robottestingframework_fixturemanager_yarpserver
  EXPORT YARP_RobotTestingFramework_plugins
  COMPONENT YARP_RobotTestingFramework_plugins
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/robottestingframework
)
