# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_dataplayer)

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_dataplayer
  DEPENDENCIES ${YARP_dataplayer_PUBLIC_DEPS}
  PRIVATE_DEPENDENCIES ${YARP_dataplayer_PRIVATE_DEPS}
)
