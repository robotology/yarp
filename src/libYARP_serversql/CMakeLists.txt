# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_serversql)

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_serversql
  DEPENDENCIES ${YARP_serversql_PUBLIC_DEPS}
  PRIVATE_DEPENDENCIES ${YARP_serversql_PRIVATE_DEPS}
)
