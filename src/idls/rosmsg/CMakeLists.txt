# Copyright: (C) 2013 iCub Facility
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

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

target_link_libraries(yarpidl_rosmsg YARP_init YARP_OS ${ACE_LIBRARIES})
install(TARGETS yarpidl_rosmsg
        EXPORT YARP
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

set_property(GLOBAL APPEND PROPERTY YARP_TOOLS yarpidl_rosmsg)

include (CMakeDependentOption)
cmake_dependent_option(TEST_yarpidl_rosmsg "Test ROS msg based IDL" OFF
                       YARP_COMPILE_TESTS OFF)
mark_as_advanced(TEST_yarpidl_rosmsg)
if (TEST_yarpidl_rosmsg)
  add_subdirectory(tests)
endif ()
