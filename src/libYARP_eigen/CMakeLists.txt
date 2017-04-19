# Copyright: (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
# Author: Silvio Traversaro
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

project(YARP_eigen)

set(YARP_eigen_HDRS include/yarp/eigen/Eigen.h)

source_group("Header Files" FILES ${YARP_eigen_HDRS})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")

add_library(YARP_eigen INTERFACE)
add_library(YARP::YARP_eigen ALIAS YARP_eigen)

if(TARGET Eigen3::Eigen) # Since Eigen 3.3.1
  target_include_directories(YARP_eigen INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                                  $<INSTALL_INTERFACE:include>)
  target_link_libraries(YARP_eigen INTERFACE YARP_sig
                                             Eigen3::Eigen)
else()
  target_include_directories(YARP_eigen INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                                  $<INSTALL_INTERFACE:include>
                                                  ${EIGEN3_INCLUDE_DIR})
  target_link_libraries(YARP_eigen INTERFACE YARP_sig)
endif()

install(TARGETS YARP_eigen
        EXPORT YARP
        COMPONENT YARP_eigen
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/eigen)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/yarp/eigen
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp)

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_eigen)
