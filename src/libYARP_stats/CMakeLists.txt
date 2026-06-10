# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_stats)

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_stats
  DEPENDENCIES ${YARP_stats_PUBLIC_DEPS}
  PRIVATE_DEPENDENCIES ${YARP_stats_PRIVATE_DEPS}
)
