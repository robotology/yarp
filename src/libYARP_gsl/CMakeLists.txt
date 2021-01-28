# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_gsl)

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_gsl
                                 DEPENDENCIES ${YARP_gsl_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_gsl_PRIVATE_DEPS})
