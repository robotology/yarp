# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_init)

set(YARP_init_HDRS)
set(YARP_init_IMPL_HDRS)
set(YARP_init_SRCS src/CustomInit.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_init_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_init_HDRS}
                   ${YARP_init_IMPL_HDRS})

add_library(YARP_init ${YARP_init_SRCS} ${YARP_init_HDRS})
add_library(YARP::YARP_init ALIAS YARP_init)

target_link_libraries(YARP_init PRIVATE YARP::YARP_conf
                                        YARP::YARP_OS)

if(YARP_LINK_PLUGINS)
    if(YARP_COMPILE_CARRIER_PLUGINS)
        set_property(SOURCE src/CustomInit.cpp
                     APPEND PROPERTY COMPILE_DEFINITIONS yarpcar_INIT_FUNCTION=add_yarpcar_plugins)
        target_link_libraries(YARP_init PRIVATE YARP::yarpcar)
#         list(APPEND YARP_init_PRIVATE_DEPS YARP_yarpcar) (This will cause circular inclusion)
    endif()

    if(YARP_COMPILE_DEVICE_PLUGINS)
        set_property(SOURCE src/CustomInit.cpp
                     APPEND PROPERTY COMPILE_DEFINITIONS yarpmod_INIT_FUNCTION=add_yarpmod_plugins)
        target_link_libraries(YARP_init PRIVATE YARP::yarpmod)
#         list(APPEND YARP_init_PRIVATE_DEPS YARP_yarpmod) (This will cause circular inclusion)
    endif()

    if(YARP_COMPILE_RFMODULE_PLUGINS)
        set_property(SOURCE src/CustomInit.cpp
                     APPEND PROPERTY COMPILE_DEFINITIONS yarprfmod_INIT_FUNCTION=add_yarprfmod_plugins)
        target_link_libraries(YARP_init PRIVATE YARP::yarprfmod)
    endif()
endif()

set_property(TARGET YARP_init PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_init PROPERTY SOVERSION ${YARP_SOVERSION})

install(TARGETS YARP_init
        EXPORT YARP_OS
        COMPONENT YARP_OS
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_OS
                                 DEPENDENCIES ${YARP_OS_PUBLIC_DEPS} ${YARP_init_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_OS_PRIVATE_DEPS} ${YARP_init_PRIVATE_DEPS}
                                 STATIC_CONFIG_TEMPLATE "${CMAKE_CURRENT_LIST_DIR}/YARP_OSConfigStatic.cmake.in")

set_property(TARGET YARP_init PROPERTY FOLDER "Libraries")
