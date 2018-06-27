# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

include_directories(src)

if(NOT SKIP_ACE)
  include_directories(SYSTEM ${ACE_INCLUDE_DIRS})
endif()

set(md5_SRCS "${md5_ROOT}/md5.c")
set(md5_HDRS "${md5_ROOT}/md5.h")

set(yarpidl_rosmsg_SRCS src/main.cpp
                        src/RosType.cpp
                        src/RosTypeCodeGenYarp.cpp)
set(yarpidl_rosmsg_HDRS src/RosType.h
                        src/RosTypeCodeGenYarp.h)

add_executable(yarpidl_rosmsg ${yarpidl_rosmsg_SRCS}
                              ${yarpidl_rosmsg_HDRS}
                              ${md5_SRCS}
                              ${md5_HDRS})
add_executable(YARP::yarpidl_rosmsg ALIAS yarpidl_rosmsg)

target_include_directories(yarpidl_rosmsg SYSTEM PRIVATE "${md5_ROOT}")
target_link_libraries(yarpidl_rosmsg PRIVATE YARP::YARP_init
                                             YARP::YARP_OS
                                             ${ACE_LIBRARIES})
install(TARGETS yarpidl_rosmsg
        EXPORT YARP
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

set_property(TARGET yarpidl_rosmsg PROPERTY FOLDER "Command Line Tools")
