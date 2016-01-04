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
target_link_libraries(yarp YARP_OS YARP_init)

if(YARP_USE_PERSISTENT_NAMESERVER)
  set(YARPSERVER_NAME yarpserver2)
else()
  set(YARPSERVER_NAME yarpserver)
endif()

add_executable(${YARPSERVER_NAME} yarpserver.cpp)
target_link_libraries(${YARPSERVER_NAME} YARP_OS YARP_init)

if(YARP_USE_PERSISTENT_NAMESERVER)
  set_property(SOURCE ${yarp_SRCS} APPEND PROPERTY COMPILE_DEFINITIONS YARP_USE_PERSISTENT_NAMESERVER=1)
  target_link_libraries(yarp YARP_serversql)
endif()

install(TARGETS yarp ${YARPSERVER_NAME} COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/extern/diff_match_patch)
add_executable(yarp-config yarpconfig.cpp yarpcontext.cpp yarpcontext.h yarprobot.cpp yarprobot.h yarpcontextutils.cpp yarpcontextutils.h)
target_link_libraries(yarp-config YARP_OS YARP_init ${ACE_LIBRARIES})
install(TARGETS yarp-config COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})
