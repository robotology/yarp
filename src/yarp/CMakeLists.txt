# Copyright: (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
include_directories(${YARP_TREE_INCLUDE_DIRS})
include_directories(${ACE_INCLUDE_DIRS})
get_filename_component(YARP_CPP yarp.cpp ABSOLUTE) # make absolute for later reference
add_executable(yarp ${YARP_CPP})
target_link_libraries(yarp YARP_OS YARP_init)

set(YARPSERVER_NAME yarpserver2)
if(YARP_USE_PERSISTENT_NAMESERVER)
  include_directories(${CMAKE_SOURCE_DIR}/src/yarpserver/include)
else()
  set(YARPSERVER_NAME yarpserver)
endif()

add_executable(${YARPSERVER_NAME} yarpserver.cpp)
target_link_libraries(${YARPSERVER_NAME} YARP_OS YARP_init)

if(YARP_USE_PERSISTENT_NAMESERVER)
  set_property(SOURCE ${YARP_CPP} APPEND PROPERTY COMPILE_DEFINITIONS YARP_USE_PERSISTENT_NAMESERVER=1)
  target_link_libraries(yarp YARP_serversql)
endif()

install(TARGETS yarp ${YARPSERVER_NAME} COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/extern/diff_match_patch)
add_executable(yarp-config yarpconfig.cpp yarpcontext.cpp yarpcontext.h yarprobot.cpp yarprobot.h yarpcontextutils.cpp yarpcontextutils.h)
target_link_libraries(yarp-config YARP_OS YARP_init ${ACE_LIBRARIES})
install(TARGETS yarp-config COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})
