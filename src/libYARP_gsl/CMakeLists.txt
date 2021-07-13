# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_gsl)

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_gsl
  DEPENDENCIES ${YARP_gsl_PUBLIC_DEPS}
  PRIVATE_DEPENDENCIES ${YARP_gsl_PRIVATE_DEPS}
)
