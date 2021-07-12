# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_library(YARP_eigen INTERFACE)
add_library(YARP::YARP_eigen ALIAS YARP_eigen)

set(YARP_eigen_HDRS yarp/eigen/Eigen.h)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_eigen_HDRS})

target_include_directories(YARP_eigen INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(YARP_eigen INTERFACE cxx_std_14)

target_link_libraries(YARP_eigen INTERFACE YARP_sig)
list(APPEND YARP_eigen_PUBLIC_DEPS YARP_sig)

target_link_libraries(YARP_eigen INTERFACE Eigen3::Eigen)
list(APPEND YARP_eigen_PUBLIC_DEPS Eigen3)

# set_property(TARGET YARP_eigen PROPERTY PUBLIC_HEADER ${YARP_eigen_HDRS})
# set_property(TARGET YARP_eigen PROPERTY PRIVATE_HEADER ${YARP_eigen_IMPL_HDRS})
# set_property(TARGET YARP_eigen PROPERTY VERSION ${YARP_VERSION_SHORT})
# set_property(TARGET YARP_eigen PROPERTY SOVERSION ${YARP_SOVERSION})
# set_property(TARGET YARP_eigen PROPERTY FOLDER "Libraries")


install(TARGETS YARP_eigen
        EXPORT YARP_eigen)

# INTERFACE libraries do not support the PUBLIC_HEADER property
# https://gitlab.kitware.com/cmake/cmake/issues/20056
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/yarp/eigen
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp
        COMPONENT YARP_eigen-dev)


set(YARP_eigen_PUBLIC_DEPS ${YARP_eigen_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_eigen_PRIVATE_DEPS ${YARP_eigen_PRIVATE_DEPS} PARENT_SCOPE)
