# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

add_library(YARP_companion)
add_library(YARP::YARP_companion ALIAS YARP_companion)

set(YARP_companion_HDRS
  yarp/companion/api.h
  yarp/companion/yarpcompanion.h
)

set(YARP_companion_SRCS
  yarp/companion/yarpcompanion.cpp
)

set(YARP_companion_IMPL_HDRS
  yarp/companion/impl/BottleReader.h
  yarp/companion/impl/Companion.h
)

set(YARP_companion_IMPL_SRCS
  yarp/companion/impl/Companion.cpp
  yarp/companion/impl/Companion.cmdCheck.cpp
  yarp/companion/impl/Companion.cmdClean.cpp
  yarp/companion/impl/Companion.cmdClock.cpp
  yarp/companion/impl/Companion.cmdCMake.cpp
  yarp/companion/impl/Companion.cmdConf.cpp
  yarp/companion/impl/Companion.cmdConnect.cpp
  yarp/companion/impl/Companion.cmdDetect.cpp
  yarp/companion/impl/Companion.cmdDisconnect.cpp
  yarp/companion/impl/Companion.cmdEnv.cpp
  yarp/companion/impl/Companion.cmdExists.cpp
  yarp/companion/impl/Companion.cmdHelp.cpp
  yarp/companion/impl/Companion.cmdLatencyTest.cpp
  yarp/companion/impl/Companion.cmdMerge.cpp
  yarp/companion/impl/Companion.cmdName.cpp
  yarp/companion/impl/Companion.cmdNamespace.cpp
  yarp/companion/impl/Companion.cmdPing.cpp
  yarp/companion/impl/Companion.cmdPlugin.cpp
  yarp/companion/impl/Companion.cmdPray.cpp
  yarp/companion/impl/Companion.cmdPriorityQos.cpp
  yarp/companion/impl/Companion.cmdPrioritySched.cpp
  yarp/companion/impl/Companion.cmdResource.cpp
  yarp/companion/impl/Companion.cmdRead.cpp
  yarp/companion/impl/Companion.cmdReadWrite.cpp
  yarp/companion/impl/Companion.cmdRepeat.cpp
  yarp/companion/impl/Companion.cmdRpc.cpp
  yarp/companion/impl/Companion.cmdRpcServer.cpp
  yarp/companion/impl/Companion.cmdSample.cpp
  yarp/companion/impl/Companion.cmdStats.cpp
  yarp/companion/impl/Companion.cmdTerminate.cpp
  yarp/companion/impl/Companion.cmdTime.cpp
  yarp/companion/impl/Companion.cmdTrafficGen.cpp
  yarp/companion/impl/Companion.cmdTopic.cpp
  yarp/companion/impl/Companion.cmdVersion.cpp
  yarp/companion/impl/Companion.cmdWait.cpp
  yarp/companion/impl/Companion.cmdWhere.cpp
  yarp/companion/impl/Companion.cmdWrite.cpp
)

source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}"
  PREFIX "Source Files"
  FILES
    ${YARP_companion_SRCS}
    ${YARP_companion_IMPL_SRCS}
)
source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}"
  PREFIX "Header Files"
  FILES
    ${YARP_companion_HDRS}
    ${YARP_companion_IMPL_HDRS}
)

target_sources(YARP_companion
  PRIVATE
    ${YARP_companion_SRCS}
    ${YARP_companion_HDRS}
    ${YARP_companion_IMPL_SRCS}
    ${YARP_companion_IMPL_HDRS}
)

target_include_directories(YARP_companion
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(YARP_companion PUBLIC YARP::YARP_os)
list(APPEND YARP_companion_PUBLIC_DEPS YARP_os)

target_compile_features(YARP_companion PUBLIC cxx_std_14)

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
set_property(TARGET YARP_companion PROPERTY FOLDER "Libraries/Private")


install(
  TARGETS YARP_companion
  EXPORT YARP_companion
  RUNTIME
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    COMPONENT YARP_companion
  LIBRARY
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT YARP_companion
    NAMELINK_COMPONENT YARP_companion-dev
  ARCHIVE
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT YARP_companion-dev
  PUBLIC_HEADER
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/companion"
    COMPONENT YARP_companion-dev
  PRIVATE_HEADER
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/companion/impl"
    COMPONENT YARP_companion-priv-dev
)


set(YARP_companion_PUBLIC_DEPS ${YARP_companion_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_companion_PRIVATE_DEPS ${YARP_companion_PRIVATE_DEPS} PARENT_SCOPE)
