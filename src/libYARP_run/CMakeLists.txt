# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_run)

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_run
                                 DEPENDENCIES ${YARP_run_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_run_PRIVATE_DEPS})
