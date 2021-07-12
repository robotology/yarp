# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_library(YARP_pcl INTERFACE)
add_library(YARP::YARP_pcl ALIAS YARP_pcl)

set(YARP_pcl_HDRS yarp/pcl/Pcl.h)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_pcl_HDRS})

target_include_directories(YARP_pcl INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                              $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(YARP_pcl INTERFACE cxx_std_14)

target_link_libraries(YARP_pcl INTERFACE YARP::YARP_sig)
list(APPEND YARP_pcl_PUBLIC_DEPS YARP_sig)

target_include_directories(YARP_pcl SYSTEM INTERFACE ${PCL_INCLUDE_DIRS})
target_link_libraries(YARP_pcl INTERFACE ${PCL_LIBRARIES})
list(APPEND YARP_pcl_PUBLIC_DEPS "PCL COMPONENTS common io")

# set_property(TARGET YARP_pcl PROPERTY PUBLIC_HEADER ${YARP_pcl_HDRS})
# set_property(TARGET YARP_pcl PROPERTY PRIVATE_HEADER ${YARP_pcl_IMPL_HDRS})
# set_property(TARGET YARP_pcl PROPERTY VERSION ${YARP_VERSION_SHORT})
# set_property(TARGET YARP_pcl PROPERTY SOVERSION ${YARP_SOVERSION})
# set_property(TARGET YARP_pcl PROPERTY FOLDER "Libraries")


# INTERFACE libraries do not support the PUBLIC_HEADER property
# https://gitlab.kitware.com/cmake/cmake/issues/20056
install(TARGETS YARP_pcl
        EXPORT YARP_pcl)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/yarp/pcl
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp
        COMPONENT YARP_pcl)


set(YARP_pcl_PUBLIC_DEPS ${YARP_pcl_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_pcl_PRIVATE_DEPS ${YARP_pcl_PRIVATE_DEPS} PARENT_SCOPE)
