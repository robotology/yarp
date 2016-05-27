# Copyright: (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
get_property(YARP_serversql_INCLUDE_DIRS TARGET YARP_serversql PROPERTY INCLUDE_DIRS)

include_directories(${YARP_OS_INCLUDE_DIRS}
                    ${YARP_serversql_INCLUDE_DIRS}
                    ${ACE_INCLUDE_DIRS})

set(yarp_SRCS yarp.cpp)

add_executable(yarp ${yarp_SRCS})
target_link_libraries(yarp YARP_OS
                           YARP_init
                           YARP_serversql)

install(TARGETS yarp
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})


set(yarp_config_SRCS yarpconfig.cpp
                     yarpcontext.cpp
                     yarprobot.cpp
                     yarpcontextutils.cpp)

set(yarp_config_HDRS yarpcontext.h
                     yarprobot.h
                     yarpcontextutils.h)

include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/extern/diff_match_patch)

add_executable(yarp-config ${yarp_config_SRCS}
                           ${yarp_config_HDRS})
target_link_libraries(yarp-config YARP_OS
                                  YARP_init
                                  ${ACE_LIBRARIES})
install(TARGETS yarp-config
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})
