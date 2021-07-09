# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_library(YARP_run)
add_library(YARP::YARP_run ALIAS YARP_run)

set(YARP_run_HDRS yarp/run/Run.h)

set(YARP_run_IMPL_HDRS yarp/run/impl/RunCheckpoints.h
                       yarp/run/impl/RunProcManager.h
                       yarp/run/impl/RunReadWrite.h
                       yarp/run/impl/PlatformSysPrctl.h
                       yarp/run/impl/PlatformStdlib.h
                       yarp/run/impl/PlatformUnistd.h)

if(NOT YARP_NO_DEPRECATED) # Since YARP 3.0.0
  set(YARP_run_os_HDRS yarp/os/Run.h)
endif()

set(YARP_run_SRCS yarp/run/Run.cpp)

set(YARP_run_IMPL_SRCS yarp/run/impl/RunCheckpoints.cpp
                       yarp/run/impl/RunProcManager.cpp
                       yarp/run/impl/RunReadWrite.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_run_SRCS}
                   ${YARP_run_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_run_HDRS}
                   ${YARP_run_IMPL_HDRS}
                   ${YARP_run_os_HDRS})

target_sources(YARP_run PRIVATE ${YARP_run_SRCS}
                                ${YARP_run_HDRS}
                                ${YARP_run_IMPL_SRCS}
                                ${YARP_run_IMPL_HDRS}
                                ${YARP_run_os_HDRS})

target_include_directories(YARP_run PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(YARP_run PUBLIC cxx_std_14)

option(YARP_ENABLE_YARPRUN_LOG "Enable yarprun log file in temp dir" OFF)
mark_as_advanced(YARP_ENABLE_YARPRUN_LOG)
if(YARP_ENABLE_YARPRUN_LOG)
   target_compile_definitions(YARP_run PRIVATE YARPRUN_LOG)
endif()
include(YarpRenamedOption)
yarp_renamed_option(ENABLE_YARPRUN_LOG YARP_ENABLE_YARPRUN_LOG) # Since YARP 3.3

target_link_libraries(YARP_run PRIVATE YARP::YARP_os)
list(APPEND YARP_run_PRIVATE_DEPS YARP_os)

if(YARP_HAS_ACE)
  target_compile_definitions(YARP_run PRIVATE YARP_HAS_ACE)
  target_link_libraries(YARP_run PRIVATE ACE::ACE_INLINE)
  list(APPEND YARP_run_PRIVATE_DEPS ACE)
endif()

set_property(TARGET YARP_run PROPERTY PUBLIC_HEADER ${YARP_run_HDRS})
set_property(TARGET YARP_run PROPERTY PRIVATE_HEADER ${YARP_run_IMPL_HDRS})
set_property(TARGET YARP_run PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_run PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_run PROPERTY FOLDER "Libraries/Private")


install(TARGETS YARP_run
        EXPORT YARP_run
        RUNTIME
          DESTINATION "${CMAKE_INSTALL_BINDIR}"
          COMPONENT YARP_run
        LIBRARY
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_run
          NAMELINK_COMPONENT YARP_run-dev
        ARCHIVE
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_run-dev
        PUBLIC_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/run"
          COMPONENT YARP_run-dev
        PRIVATE_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/run/impl"
          COMPONENT YARP_run-priv-dev)
if(NOT YARP_NO_DEPRECATED) # Since YARP 3.0.0
  install(FILES ${YARP_run_os_HDRS}
          COMPONENT YARP_os-dev
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/os")
endif()


set(YARP_run_PUBLIC_DEPS ${YARP_run_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_run_PRIVATE_DEPS ${YARP_run_PRIVATE_DEPS} PARENT_SCOPE)
