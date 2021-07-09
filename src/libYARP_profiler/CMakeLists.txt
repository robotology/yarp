# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_profiler)

if(NOT YARP_COMPILE_libYARP_profiler)
  return()
endif()

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_profiler
                                 DEPENDENCIES ${YARP_profiler_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_profiler_PRIVATE_DEPS})
