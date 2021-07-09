# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_wire_rep_utils)

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_wire_rep_utils
                                 DEPENDENCIES ${YARP_wire_rep_utils_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_wire_rep_utils_PRIVATE_DEPS})
