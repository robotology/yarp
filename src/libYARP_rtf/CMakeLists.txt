# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_rtf)


set(YARP_rtf_HDRS include/yarp/rtf/JointsPosMotion.h
                  include/yarp/rtf/TestAsserter.h
                  include/yarp/rtf/TestCase.h
                  include/yarp/rtf/api.h)

set(YARP_rtf_SRCS src/JointsPosMotion.cpp
                  src/TestAsserter.cpp
                  src/TestCase.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_rtf_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_rtf_HDRS})

add_library(YARP_rtf ${YARP_rtf_SRCS} ${YARP_rtf_HDRS})
add_library(YARP::YARP_rtf ALIAS YARP_rtf)

target_include_directories(YARP_rtf PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(YARP_rtf PUBLIC YARP::YARP_conf
                                      YARP::YARP_OS
                                      YARP::YARP_sig
                                      YARP::YARP_dev
                               PRIVATE YARP::YARP_init)
list(APPEND YARP_rtf_PUBLIC_DEPS YARP_conf
                                 YARP_OS
                                 YARP_sig
                                 YARP_dev)

target_include_directories(YARP_rtf SYSTEM PUBLIC ${RTF_INCLUDE_DIRS})
target_link_libraries(YARP_rtf PUBLIC RTF::RTF)
list(APPEND YARP_rtf_PUBLIC_DEPS RTF)

target_compile_features(YARP_rtf PUBLIC cxx_nullptr
                                        cxx_override)

set_property(TARGET YARP_rtf PROPERTY PUBLIC_HEADER ${YARP_rtf_HDRS})
set_property(TARGET YARP_rtf PROPERTY PRIVATE_HEADER ${YARP_rtf_IMPL_HDRS})
set_property(TARGET YARP_rtf PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_rtf PROPERTY SOVERSION ${YARP_SOVERSION})

install(TARGETS YARP_rtf
        EXPORT YARP_rtf
        COMPONENT YARP_rtf
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/rtf)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_rtf
                                 DEPENDENCIES ${YARP_rtf_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_rtf_PRIVATE_DEPS})

set_property(TARGET YARP_rtf PROPERTY FOLDER "Libraries")
