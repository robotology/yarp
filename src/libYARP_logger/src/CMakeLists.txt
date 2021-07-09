# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# FIXME All API should use a YARP_logger_API for __declspec(dllimport/dllexport)
# For now always build the library as STATIC
add_library(YARP_logger STATIC)
add_library(YARP::YARP_logger ALIAS YARP_logger)

set(YARP_logger_HDRS yarp/logger/YarpLogger.h)

set(YARP_logger_IMPL_HDRS )

set(YARP_logger_SRCS yarp/logger/YarpLogger.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_logger_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_logger_HDRS}
                   ${YARP_logger_IMPL_HDRS})

target_sources(YARP_logger PRIVATE ${YARP_logger_SRCS}
                                   ${YARP_logger_HDRS}
                                   ${YARP_logger_IMPL_HDRS})

target_include_directories(YARP_logger PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                              $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(YARP_logger PUBLIC cxx_std_14)

target_link_libraries(YARP_logger PUBLIC YARP::YARP_os
                                  PRIVATE YARP::YARP_sig)
list(APPEND YARP_logger_PUBLIC_DEPS YARP_os)
list(APPEND YARP_logger_PRIVATE_DEPS YARP_sig)

set_property(TARGET YARP_logger PROPERTY PUBLIC_HEADER ${YARP_logger_HDRS})
set_property(TARGET YARP_logger PROPERTY PRIVATE_HEADER ${YARP_logger_IMPL_HDRS})
set_property(TARGET YARP_logger PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_logger PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_logger PROPERTY FOLDER "Libraries/Private")


install(TARGETS YARP_logger
        EXPORT YARP_logger
        RUNTIME
          DESTINATION "${CMAKE_INSTALL_BINDIR}"
          COMPONENT YARP_logger
        LIBRARY
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_logger
          NAMELINK_COMPONENT YARP_logger-dev
        ARCHIVE
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_logger-dev
        PUBLIC_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/logger"
          COMPONENT YARP_logger-dev
        PRIVATE_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/logger/impl"
          COMPONENT YARP_logger-priv-dev)


set(YARP_logger_PUBLIC_DEPS ${YARP_logger_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_logger_PRIVATE_DEPS ${YARP_logger_PRIVATE_DEPS} PARENT_SCOPE)
