# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_init)

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_os
  DEPENDENCIES ${YARP_os_PUBLIC_DEPS} ${YARP_init_PUBLIC_DEPS}
  PRIVATE_DEPENDENCIES ${YARP_os_PRIVATE_DEPS} ${YARP_init_PRIVATE_DEPS}
  STATIC_CONFIG_TEMPLATE "${CMAKE_CURRENT_LIST_DIR}/YARP_osConfigStatic.cmake.in"
  INCLUDE_CONTENT "
if(NOT YARP_NO_DEPRECATED AND NOT TARGET YARP::YARP_OS)
  add_library(YARP::YARP_OS INTERFACE IMPORTED)
  set_target_properties(YARP::YARP_OS PROPERTIES
    INTERFACE_LINK_LIBRARIES \"YARP::YARP_os\"
    INTERFACE_COMPILE_DEFINITIONS USING_DEPRECATED_UPPERCASE_YARP_OS_TARGET
  )
endif()
"
)
