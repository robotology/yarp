# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

include_directories(src)

get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
include_directories(${YARP_OS_INCLUDE_DIRS})

if(NOT SKIP_ACE)
  include_directories(SYSTEM ${ACE_INCLUDE_DIRS})
endif()

set(yarpidl_rosmsg_SRCS src/main.cpp
                        src/RosType.cpp
                        src/RosTypeCodeGenYarp.cpp
                        src/md5.c)
set(yarpidl_rosmsg_HDRS src/RosType.h
                        src/RosTypeCodeGenYarp.h
                        src/md5.h)

add_executable(yarpidl_rosmsg ${yarpidl_rosmsg_SRCS}
                              ${yarpidl_rosmsg_HDRS})
add_executable(YARP::yarpidl_rosmsg ALIAS yarpidl_rosmsg)

target_link_libraries(yarpidl_rosmsg YARP::YARP_init
                                     YARP::YARP_OS
                                     ${ACE_LIBRARIES})
install(TARGETS yarpidl_rosmsg
        EXPORT YARP
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

set_property(GLOBAL APPEND PROPERTY YARP_TOOLS yarpidl_rosmsg)

set_property(TARGET yarpidl_rosmsg PROPERTY FOLDER "Command Line Tools")
