# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
get_property(YARP_serversql_INCLUDE_DIRS TARGET YARP_serversql PROPERTY INCLUDE_DIRS)

include_directories(${YARP_OS_INCLUDE_DIRS}
                    ${YARP_serversql_INCLUDE_DIRS}
                    ${ACE_INCLUDE_DIRS})

set(yarp_SRCS yarp.cpp)

add_executable(yarp ${yarp_SRCS})
target_link_libraries(yarp YARP::YARP_OS
                           YARP::YARP_init
                           YARP::YARP_serversql)

install(TARGETS yarp
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

set_property(TARGET yarp PROPERTY FOLDER "Command Line Tools")

set(yarp_config_SRCS yarpconfig.cpp
                     yarpcontext.cpp
                     yarprobot.cpp
                     yarpcontextutils.cpp)

set(yarp_config_HDRS yarpcontext.h
                     yarprobot.h
                     yarpcontextutils.h)

include_directories(SYSTEM ${diff_match_patch_INCLUDE_DIRS})

add_executable(yarp-config ${yarp_config_SRCS}
                           ${yarp_config_HDRS})
target_link_libraries(yarp-config YARP::YARP_OS
                                  YARP::YARP_init
                                  ${ACE_LIBRARIES})
install(TARGETS yarp-config
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

set_property(TARGET yarp-config PROPERTY FOLDER "Command Line Tools")
