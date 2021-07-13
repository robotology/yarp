# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

set(yarpserver_SRCS src/main.cpp)

add_executable(yarpserver)
target_sources(yarpserver PRIVATE ${yarpserver_SRCS})
target_link_libraries(yarpserver PRIVATE YARP::YARP_serversql)

install(
  TARGETS yarpserver
  COMPONENT utilities
  DESTINATION ${CMAKE_INSTALL_BINDIR}
)
set_property(TARGET yarpserver PROPERTY FOLDER "Command Line Tools")
