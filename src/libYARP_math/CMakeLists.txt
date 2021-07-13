# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_math)

if(NOT YARP_COMPILE_libYARP_math)
  return()
endif()

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_math
  DEPENDENCIES ${YARP_math_PUBLIC_DEPS}
  PRIVATE_DEPENDENCIES ${YARP_math_PRIVATE_DEPS}
)
