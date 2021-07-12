# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

add_library(YARP_init)
add_library(YARP::YARP_init ALIAS YARP_init)

set(YARP_init_HDRS)
set(YARP_init_IMPL_HDRS)
set(YARP_init_SRCS yarp/os/Network.CustomInit.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_init_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_init_HDRS}
                   ${YARP_init_IMPL_HDRS})

target_sources(YARP_init PRIVATE ${YARP_init_SRCS}
                                 ${YARP_init_HDRS})

target_link_libraries(YARP_init PRIVATE YARP::YARP_conf
                                        YARP::YARP_os)
target_compile_features(YARP_init PUBLIC cxx_std_14)

if(YARP_LINK_PLUGINS)
    if(YARP_COMPILE_CARRIER_PLUGINS)
        set_property(SOURCE CustomInit.cpp
                     APPEND PROPERTY COMPILE_DEFINITIONS yarpcar_INIT_FUNCTION=add_yarpcar_plugins)
        target_link_libraries(YARP_init PRIVATE YARP::yarpcar)
#         list(APPEND YARP_init_PRIVATE_DEPS YARP_yarpcar) (This will cause circular inclusion)
    endif()

    if(YARP_COMPILE_PORTMONITOR_PLUGINS)
        set_property(SOURCE CustomInit.cpp
                     APPEND PROPERTY COMPILE_DEFINITIONS yarppm_INIT_FUNCTION=add_yarppm_plugins)
        target_link_libraries(YARP_init PRIVATE YARP::yarppm)
#         list(APPEND YARP_init_PRIVATE_DEPS YARP_yarppm) (This will cause circular inclusion)
    endif()

    if(YARP_COMPILE_DEVICE_PLUGINS)
        set_property(SOURCE CustomInit.cpp
                     APPEND PROPERTY COMPILE_DEFINITIONS yarpmod_INIT_FUNCTION=add_yarpmod_plugins)
        target_link_libraries(YARP_init PRIVATE YARP::yarpmod)
#         list(APPEND YARP_init_PRIVATE_DEPS YARP_yarpmod) (This will cause circular inclusion)
    endif()

    if(YARP_COMPILE_RFMODULE_PLUGINS)
        set_property(SOURCE CustomInit.cpp
                     APPEND PROPERTY COMPILE_DEFINITIONS yarprfmod_INIT_FUNCTION=add_yarprfmod_plugins)
        target_link_libraries(YARP_init PRIVATE YARP::yarprfmod)
    endif()
endif()

set_property(TARGET YARP_init PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_init PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_init PROPERTY FOLDER "Libraries")

install(TARGETS YARP_init
        EXPORT YARP_os
        COMPONENT YARP_os
        RUNTIME
          DESTINATION "${CMAKE_INSTALL_BINDIR}"
          COMPONENT YARP_os
        LIBRARY
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_os-dev
        ARCHIVE
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_os-dev)
