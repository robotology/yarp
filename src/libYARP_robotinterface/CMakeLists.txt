# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_robotinterface)

if(NOT YARP_COMPILE_libYARP_robotinterface)
  return()
endif()

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_robotinterface
                                 DEPENDENCIES ${YARP_robotinterface_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_robotinterface_PRIVATE_DEPS})
