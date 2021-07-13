# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

add_library(YARP_wire_rep_utils STATIC)
add_library(YARP::YARP_wire_rep_utils ALIAS YARP_wire_rep_utils)

set(YARP_wire_rep_utils_HDRS
  yarp/wire_rep_utils/api.h
  yarp/wire_rep_utils/BlobNetworkHeader.h
  yarp/wire_rep_utils/WireImage.h
  yarp/wire_rep_utils/WireBottle.h
  yarp/wire_rep_utils/WireTwiddler.h
)

set(YARP_wire_rep_utils_SRCS
  yarp/wire_rep_utils/WireImage.cpp
  yarp/wire_rep_utils/WireBottle.cpp
  yarp/wire_rep_utils/WireTwiddler.cpp
)

target_sources(YARP_wire_rep_utils
  PRIVATE
    ${YARP_wire_rep_utils_HDRS}
    ${YARP_wire_rep_utils_SRCS}
)

target_include_directories(YARP_wire_rep_utils
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(YARP_wire_rep_utils PUBLIC cxx_std_14)

target_link_libraries(YARP_wire_rep_utils
  PUBLIC
    YARP::YARP_conf
    YARP::YARP_os
    YARP::YARP_sig
)
list(APPEND YARP_wire_rep_utils_PUBLIC_DEPS
  YARP_conf
  YARP_os
  YARP_sig
)

if(YARP_HAS_ACE)
  target_compile_definitions(YARP_wire_rep_utils PRIVATE YARP_HAS_ACE)
  target_link_libraries(YARP_wire_rep_utils PRIVATE ACE::ACE_INLINE)
  list(APPEND YARP_wire_rep_utils_PRIVATE_DEPS ACE)
endif()

set_property(TARGET YARP_wire_rep_utils PROPERTY PUBLIC_HEADER ${YARP_wire_rep_utils_HDRS})
set_property(TARGET YARP_wire_rep_utils PROPERTY PRIVATE_HEADER ${YARP_wire_rep_utils_IMPL_HDRS})
set_property(TARGET YARP_wire_rep_utils PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_wire_rep_utils PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_wire_rep_utils PROPERTY FOLDER "Libraries/Private")


install(
  TARGETS YARP_wire_rep_utils
  EXPORT YARP_wire_rep_utils
  RUNTIME
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    COMPONENT YARP_wire_rep_utils
  LIBRARY
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT YARP_wire_rep_utils
    NAMELINK_COMPONENT YARP_wire_rep_utils-dev
  ARCHIVE
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT YARP_wire_rep_utils-dev
  PUBLIC_HEADER
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/wire_rep_utils"
    COMPONENT YARP_wire_rep_utils-dev
  PRIVATE_HEADER
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/wire_rep_utils/impl"
    COMPONENT YARP_wire_rep_utils-priv-dev
)


set(YARP_wire_rep_utils_PUBLIC_DEPS ${YARP_wire_rep_utils_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_wire_rep_utils_PRIVATE_DEPS ${YARP_wire_rep_utils_PRIVATE_DEPS} PARENT_SCOPE)
