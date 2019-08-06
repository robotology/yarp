# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_robottestingframework)


set(YARP_robottestingframework_HDRS include/yarp/robottestingframework/JointsPosMotion.h
                                    include/yarp/robottestingframework/TestAsserter.h
                                    include/yarp/robottestingframework/TestCase.h
                                    include/yarp/robottestingframework/api.h)

set(YARP_robottestingframework_SRCS src/JointsPosMotion.cpp
                                    src/TestAsserter.cpp
                                    src/TestCase.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_robottestingframework_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_robottestingframework_HDRS})

add_library(YARP_robottestingframework ${YARP_robottestingframework_SRCS} ${YARP_robottestingframework_HDRS})
add_library(YARP::YARP_robottestingframework ALIAS YARP_robottestingframework)

target_include_directories(YARP_robottestingframework PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                                             $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(YARP_robottestingframework PUBLIC YARP::YARP_conf
                                                        YARP::YARP_os
                                                        YARP::YARP_sig
                                                        YARP::YARP_dev
                                                 PRIVATE YARP::YARP_init)
list(APPEND YARP_robottestingframework_PUBLIC_DEPS YARP_conf
                                                   YARP_os
                                                   YARP_sig
                                                   YARP_dev)

target_link_libraries(YARP_robottestingframework PUBLIC RobotTestingFramework::RTF)
list(APPEND YARP_robottestingframework_PUBLIC_DEPS RobotTestingFramework)

target_compile_features(YARP_robottestingframework PUBLIC cxx_nullptr
                                        cxx_override)

set_property(TARGET YARP_robottestingframework PROPERTY PUBLIC_HEADER ${YARP_robottestingframework_HDRS})
set_property(TARGET YARP_robottestingframework PROPERTY PRIVATE_HEADER ${YARP_robottestingframework_IMPL_HDRS})
set_property(TARGET YARP_robottestingframework PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_robottestingframework PROPERTY SOVERSION ${YARP_SOVERSION})

install(TARGETS YARP_robottestingframework
        EXPORT YARP_robottestingframework
        COMPONENT YARP_robottestingframework
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/robottestingframework)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_robottestingframework
                                 DEPENDENCIES ${YARP_robottestingframework_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_robottestingframework_PRIVATE_DEPS})

set_property(TARGET YARP_robottestingframework PROPERTY FOLDER "Libraries")
