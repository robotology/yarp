# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_companion)

set(YARP_companion_HDRS include/yarp/companion/api.h
                        include/yarp/companion/yarpcompanion.h)

set(YARP_companion_IMPL_HDRS include/yarp/companion/impl/Companion.h)

set(YARP_companion_SRCS src/Companion.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_companion_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_companion_HDRS}
                   ${YARP_companion_IMPL_HDRS})

add_library(YARP_companion ${YARP_companion_SRCS}
                           ${YARP_companion_HDRS}
                           ${YARP_companion_IMPL_HDRS})
add_library(YARP::YARP_companion ALIAS YARP_companion)

target_include_directories(YARP_companion PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                                 $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(YARP_companion PUBLIC YARP::YARP_os)
list(APPEND YARP_companion_PUBLIC_DEPS YARP_os)

target_compile_features(YARP_companion PUBLIC cxx_override)

if(YARP_HAS_ACE)
  target_compile_definitions(YARP_companion PRIVATE YARP_HAS_ACE)
  target_link_libraries(YARP_companion PRIVATE ACE::ACE_INLINE)
  list(APPEND YARP_companion_PRIVATE_DEPS ACE)
endif()

if(YARP_HAS_Libedit)
  target_compile_definitions(YARP_companion PRIVATE YARP_HAS_Libedit)
  target_link_libraries(YARP_companion PRIVATE Libedit::libedit)
  list(APPEND YARP_companion_PRIVATE_DEPS Libedit)
endif()

set_property(TARGET YARP_companion PROPERTY PUBLIC_HEADER ${YARP_companion_HDRS})
set_property(TARGET YARP_companion PROPERTY PRIVATE_HEADER ${YARP_companion_IMPL_HDRS})
set_property(TARGET YARP_companion PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_companion PROPERTY SOVERSION ${YARP_SOVERSION})

install(TARGETS YARP_companion
        EXPORT YARP_companion
        COMPONENT YARP_companion
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/companion"
        PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/companion/impl")

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_companion
                                 DEPENDENCIES ${YARP_companion_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_companion_PRIVATE_DEPS})

set_property(TARGET YARP_companion PROPERTY FOLDER "Libraries")
