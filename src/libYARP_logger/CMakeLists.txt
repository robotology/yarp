# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_logger)

set(YARP_logger_HDRS include/yarp/logger/YarpLogger.h)

set(YARP_logger_IMPL_HDRS )

set(YARP_logger_SRCS src/YarpLogger.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_logger_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_logger_HDRS}
                   ${YARP_logger_IMPL_HDRS})


# FIXME All API should use a YARP_logger_API for __declspec(dllimport/dllexport)
# For now always build the library as STATIC
add_library(YARP_logger STATIC ${YARP_logger_SRCS}
                               ${YARP_logger_HDRS}
                               ${YARP_logger_IMPL_HDRS})
add_library(YARP::YARP_logger ALIAS YARP_logger)

target_include_directories(YARP_logger PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                              $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(YARP_logger PUBLIC YARP::YARP_os
                                  PRIVATE YARP::YARP_sig)
list(APPEND YARP_logger_PUBLIC_DEPS YARP_os)
list(APPEND YARP_logger_PRIVATE_DEPS YARP_sig)

target_compile_features(YARP_logger PUBLIC cxx_override)

set_property(TARGET YARP_logger PROPERTY PUBLIC_HEADER ${YARP_logger_HDRS})
set_property(TARGET YARP_logger PROPERTY PRIVATE_HEADER ${YARP_logger_IMPL_HDRS})
set_property(TARGET YARP_logger PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_logger PROPERTY SOVERSION ${YARP_SOVERSION})

install(TARGETS YARP_logger
        EXPORT YARP_logger
        COMPONENT YARP_logger
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/logger"
        PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/logger/impl")

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_logger
                                 DEPENDENCIES ${YARP_logger_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_logger_PRIVATE_DEPS})

set_property(TARGET YARP_logger PROPERTY FOLDER "Libraries/Private")
