# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_manager)

if(NOT YARP_COMPILE_libYARP_manager)
  return()
endif()

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_manager
                                 DEPENDENCIES ${YARP_manager_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_manager_PRIVATE_DEPS})
