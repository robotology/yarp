# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

add_executable(yarphear)
target_sources(yarphear
  PRIVATE
    yarphear.cpp
)
target_link_libraries(yarphear
  PRIVATE
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_dev
    YARP::YARP_init
)

install(
  TARGETS yarphear
  COMPONENT utilities
  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set_property(TARGET yarphear PROPERTY FOLDER "Command Line Tools")
