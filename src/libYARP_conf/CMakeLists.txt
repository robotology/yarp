# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_conf)

add_subdirectory(src)

# Install cmake files
include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_conf
                                 DEPENDENCIES ${YARP_conf_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_conf_PRIVATE_DEPS})
