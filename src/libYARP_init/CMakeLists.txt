# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

project(YARP_init)

get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
get_property(YARP_TREE_LIBRARIES GLOBAL PROPERTY YARP_LIBS)
include_directories(${YARP_TREE_INCLUDE_DIRS})

set(YARP_init_SRCS src/CustomInit.cpp)
add_library(YARP_init ${YARP_init_SRCS})
target_link_libraries(YARP_init LINK_PRIVATE YARP_OS)

if (CREATE_OPTIONAL_CARRIERS)
    set_property(SOURCE src/CustomInit.cpp
                 APPEND PROPERTY COMPILE_DEFINITIONS
                     PLUGIN_INIT_FUNCTION=add_yarpcar_devices)
    target_link_libraries(YARP_init LINK_PRIVATE yarpcar)
endif (CREATE_OPTIONAL_CARRIERS)

if (CREATE_DEVICE_LIBRARY_MODULES)
    set_property(SOURCE src/CustomInit.cpp
                 APPEND PROPERTY COMPILE_DEFINITIONS
                     PLUGIN_INIT_FUNCTION2=add_yarpmod_devices)
    target_link_libraries(YARP_init LINK_PRIVATE yarpmod)
    target_link_libraries(YARP_init LINK_PRIVATE YARP_dev)
endif (CREATE_DEVICE_LIBRARY_MODULES)

install(TARGETS YARP_init EXPORT YARP COMPONENT runtime DESTINATION lib${LIB_SUFFIX})

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_init)

set_property(TARGET YARP_init PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})
