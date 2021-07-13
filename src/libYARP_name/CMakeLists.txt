# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_name)

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_name
  DEPENDENCIES ${YARP_name_PUBLIC_DEPS}
  PRIVATE_DEPENDENCIES ${YARP_name_PRIVATE_DEPS}
)
