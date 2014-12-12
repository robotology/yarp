# Copyright: (C) 2013 iCub Facility
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
include_directories(${YARP_TREE_INCLUDE_DIRS})
if (NOT SKIP_ACE)
  include_directories(${ACE_INCLUDE_DIRS})
endif ()

include_directories(src)
add_executable(yarpidl_rosmsg src/main.cpp src/RosType.cpp src/RosTypeCodeGenYarp.cpp src/RosType.h src/RosTypeCodeGenYarp.h src/md5.c src/md5.h)

target_link_libraries(yarpidl_rosmsg YARP_init YARP_OS ${ACE_LIBRARIES})
install(TARGETS yarpidl_rosmsg
        EXPORT YARP
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

set_property(GLOBAL APPEND PROPERTY YARP_TOOLS yarpidl_rosmsg)

option(TEST_yarpidl_rosmsg "Test ROS msg based IDL" OFF)
if (TEST_yarpidl_rosmsg)
  add_subdirectory(tests)
endif ()
