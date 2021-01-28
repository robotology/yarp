# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_math)

if(NOT YARP_COMPILE_libYARP_math)
  return()
endif()

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_math
                                 DEPENDENCIES ${YARP_math_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_math_PRIVATE_DEPS})
