# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_library(YARP_cv INTERFACE)
add_library(YARP::YARP_cv ALIAS YARP_cv)

set(YARP_cv_HDRS yarp/cv/Cv.h
                 yarp/cv/Cv-inl.h)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_cv_HDRS})

target_include_directories(YARP_cv INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                             $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(YARP_conf INTERFACE cxx_std_14)

target_link_libraries(YARP_cv INTERFACE YARP::YARP_sig)
list(APPEND YARP_cv_PUBLIC_DEPS YARP_sig)

target_include_directories(YARP_cv SYSTEM INTERFACE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(YARP_cv INTERFACE ${OpenCV_LIBRARIES})
list(APPEND YARP_cv_PUBLIC_DEPS OpenCV)

# set_property(TARGET YARP_cv PROPERTY PUBLIC_HEADER ${YARP_cv_HDRS})
# set_property(TARGET YARP_cv PROPERTY PRIVATE_HEADER ${YARP_cv_IMPL_HDRS})
# set_property(TARGET YARP_cv PROPERTY VERSION ${YARP_VERSION_SHORT})
# set_property(TARGET YARP_cv PROPERTY SOVERSION ${YARP_SOVERSION})
# set_property(TARGET YARP_cv PROPERTY FOLDER "Libraries")


install(TARGETS YARP_cv
        EXPORT YARP_cv)

# INTERFACE libraries do not support the PUBLIC_HEADER property
# https://gitlab.kitware.com/cmake/cmake/issues/20056
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/yarp/cv
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp
        COMPONENT YARP_cv-dev)


set(YARP_cv_PUBLIC_DEPS ${YARP_cv_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_cv_PRIVATE_DEPS ${YARP_cv_PRIVATE_DEPS} PARENT_SCOPE)
