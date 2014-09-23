# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
include_directories(${YARP_TREE_INCLUDE_DIRS})
add_executable(yarpdev yarpdev.cpp)
target_link_libraries(yarpdev YARP_init YARP_dev)
if (CREATE_DEVICE_LIBRARY_MODULES)
  target_link_libraries(yarpdev yarpmod)
endif(CREATE_DEVICE_LIBRARY_MODULES)

install(TARGETS yarpdev
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})
