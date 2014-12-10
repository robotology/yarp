# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

project(YARP_init)

get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
get_property(YARP_TREE_LIBRARIES GLOBAL PROPERTY YARP_LIBS)
include_directories(${YARP_TREE_INCLUDE_DIRS})

set(YARP_init_HDRS)
set(YARP_init_IMPL_HDRS)
set(YARP_init_SRCS src/CustomInit.cpp)

source_group("Source Files" FILES ${YARP_init_SRCS})
source_group("Header Files" FILES ${YARP_init_HDRS})
source_group("Implementation Header Files" FILES ${YARP_init_IMPL_HDRS})

add_library(YARP_init ${YARP_init_SRCS} ${YARP_init_HDRS})
target_link_libraries(YARP_init LINK_PRIVATE YARP_OS)

if(YARP_LINK_PLUGINS)
    if(CREATE_OPTIONAL_CARRIERS)
        set_property(SOURCE src/CustomInit.cpp
                     APPEND PROPERTY COMPILE_DEFINITIONS
                     PLUGIN_INIT_FUNCTION=add_yarpcar_devices)
        target_link_libraries(YARP_init LINK_PRIVATE yarpcar)
    endif()

    if(CREATE_DEVICE_LIBRARY_MODULES)
        set_property(SOURCE src/CustomInit.cpp
                     APPEND PROPERTY COMPILE_DEFINITIONS
                     PLUGIN_INIT_FUNCTION2=add_yarpmod_devices)
        target_link_libraries(YARP_init LINK_PRIVATE yarpmod)
        target_link_libraries(YARP_init LINK_PRIVATE YARP_dev)
    endif()
endif()

install(TARGETS YARP_init
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_init)
set_property(TARGET YARP_init PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})
