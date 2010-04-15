# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

project(YARP_init)

get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
include_directories(${YARP_TREE_INCLUDE_DIRS})

add_library(YARP_init src/CustomInit.cpp)
target_link_libraries(YARP_init YARP_OS)
install(TARGETS YARP_init EXPORT YARP COMPONENT runtime DESTINATION lib)

if (CREATE_OPTIONAL_CARRIERS)
  set_property(SOURCE init/CustomInit.cpp APPEND PROPERTY COMPILE_DEFINITIONS 
               PLUGIN_INIT_FUNCTION=add_yarpcar_devices)
  target_link_libraries(YARP_init yarpcar)
endif (CREATE_OPTIONAL_CARRIERS)

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_init)

set_property(TARGET YARP_init PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})
