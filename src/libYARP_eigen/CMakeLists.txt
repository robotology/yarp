# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_eigen)

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_eigen
                                 DEPENDENCIES ${YARP_eigen_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_eigen_PRIVATE_DEPS})
