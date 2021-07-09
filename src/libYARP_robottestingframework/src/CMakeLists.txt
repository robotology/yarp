# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_library(YARP_robottestingframework)
add_library(YARP::YARP_robottestingframework ALIAS YARP_robottestingframework)

set(YARP_robottestingframework_HDRS yarp/robottestingframework/JointsPosMotion.h
                                    yarp/robottestingframework/TestAsserter.h
                                    yarp/robottestingframework/TestCase.h
                                    yarp/robottestingframework/api.h)

set(YARP_robottestingframework_SRCS yarp/robottestingframework/JointsPosMotion.cpp
                                    yarp/robottestingframework/TestAsserter.cpp
                                    yarp/robottestingframework/TestCase.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_robottestingframework_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_robottestingframework_HDRS})

target_sources(YARP_robottestingframework PRIVATE ${YARP_robottestingframework_SRCS}
                                                  ${YARP_robottestingframework_HDRS})

target_include_directories(YARP_robottestingframework PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                                             $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(YARP_robottestingframework PUBLIC cxx_std_14)

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

set_property(TARGET YARP_robottestingframework PROPERTY PUBLIC_HEADER ${YARP_robottestingframework_HDRS})
set_property(TARGET YARP_robottestingframework PROPERTY PRIVATE_HEADER ${YARP_robottestingframework_IMPL_HDRS})
set_property(TARGET YARP_robottestingframework PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_robottestingframework PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_robottestingframework PROPERTY FOLDER "Libraries")


install(TARGETS YARP_robottestingframework
        EXPORT YARP_robottestingframework
        RUNTIME
          DESTINATION "${CMAKE_INSTALL_BINDIR}"
          COMPONENT YARP_robottestingframework
        LIBRARY
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_robottestingframework
          NAMELINK_COMPONENT YARP_robottestingframework-dev
        ARCHIVE
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_robottestingframework-dev
        PUBLIC_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/robottestingframework"
          COMPONENT YARP_robottestingframework-dev
        PRIVATE_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/robottestingframework/impl"
          COMPONENT YARP_robottestingframework-priv-dev)


set(YARP_robottestingframework_PUBLIC_DEPS ${YARP_robottestingframework_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_robottestingframework_PRIVATE_DEPS ${YARP_robottestingframework_PRIVATE_DEPS} PARENT_SCOPE)
