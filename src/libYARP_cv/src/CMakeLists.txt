# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

set(YARP_cv_HDRS yarp/cv/Cv.h
                 yarp/cv/Cv-inl.h)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_cv_HDRS})

add_library(YARP_cv INTERFACE)
add_library(YARP::YARP_cv ALIAS YARP_cv)

target_include_directories(YARP_cv INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                             $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(YARP_conf INTERFACE cxx_std_14)

target_link_libraries(YARP_cv INTERFACE YARP::YARP_sig)
list(APPEND YARP_cv_PUBLIC_DEPS YARP_sig)

target_include_directories(YARP_cv SYSTEM INTERFACE ${OPENCV_INCLUDE_DIR})
target_link_libraries(YARP_cv INTERFACE ${OPENCV_INCLUDE_DIR})
list(APPEND YARP_cv_PUBLIC_DEPS OpenCV)


install(TARGETS YARP_cv
        EXPORT YARP_cv
        COMPONENT YARP_cv
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/cv)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/yarp/cv
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp)


set(YARP_cv_PUBLIC_DEPS ${YARP_cv_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_cv_PRIVATE_DEPS ${YARP_cv_PRIVATE_DEPS} PARENT_SCOPE)
