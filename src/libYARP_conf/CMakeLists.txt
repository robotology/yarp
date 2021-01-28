# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_conf)

add_subdirectory(src)

# Install cmake files
include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_conf
                                 DEPENDENCIES ${YARP_conf_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_conf_PRIVATE_DEPS})
