# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_eigen)

set(YARP_eigen_HDRS include/yarp/eigen/Eigen.h)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_eigen_HDRS})

add_library(YARP_eigen INTERFACE)
add_library(YARP::YARP_eigen ALIAS YARP_eigen)

target_include_directories(YARP_eigen INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(YARP_eigen INTERFACE YARP_sig)
list(APPEND YARP_eigen_PUBLIC_DEPS YARP_sig)

target_link_libraries(YARP_eigen INTERFACE Eigen3::Eigen)
list(APPEND YARP_eigen_PUBLIC_DEPS Eigen3)

install(TARGETS YARP_eigen
        EXPORT YARP_eigen
        COMPONENT YARP_eigen
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/eigen)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/yarp/eigen
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_eigen
                                 DEPENDENCIES ${YARP_eigen_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_eigen_PRIVATE_DEPS})

#set_property(TARGET YARP_eigen PROPERTY FOLDER "Libraries")
