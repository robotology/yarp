# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_robottestingframework)

if(NOT YARP_COMPILE_libYARP_robottestingframework)
  return()
endif()

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_robottestingframework
  DEPENDENCIES ${YARP_robottestingframework_PUBLIC_DEPS}
  PRIVATE_DEPENDENCIES ${YARP_robottestingframework_PRIVATE_DEPS}
)
