# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

set(YARP_gsl_HDRS yarp/gsl/api.h
                  yarp/gsl/Gsl.h)
set(YARP_gsl_SRCS yarp/gsl/Gsl.cpp)
set(YARP_gsl_IMPL_HDRS yarp/gsl/impl/gsl_structs.h)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_gsl_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_gsl_HDRS}
                   ${YARP_gsl_IMPL_HDRS})

add_library(YARP_gsl ${YARP_gsl_SRCS} ${YARP_gsl_HDRS})
add_library(YARP::YARP_gsl ALIAS YARP_gsl)

target_include_directories(YARP_gsl PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(YARP_gsl PUBLIC cxx_std_14)

target_link_libraries(YARP_gsl PUBLIC YARP::YARP_sig)
list(APPEND YARP_gsl_PUBLIC_DEPS YARP_sig)

set_property(TARGET YARP_gsl PROPERTY PUBLIC_HEADER ${YARP_gsl_HDRS})
set_property(TARGET YARP_gsl PROPERTY PRIVATE_HEADER ${YARP_gsl_IMPL_HDRS})
set_property(TARGET YARP_gsl PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_gsl PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_gsl PROPERTY FOLDER "Libraries")


install(TARGETS YARP_gsl
        EXPORT YARP_gsl
        COMPONENT YARP_gsl
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/gsl
        PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/gsl/impl)


set(YARP_gsl_PUBLIC_DEPS ${YARP_gsl_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_gsl_PRIVATE_DEPS ${YARP_gsl_PRIVATE_DEPS} PARENT_SCOPE)