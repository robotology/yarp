# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_logger)

add_subdirectory(src)

if(YARP_COMPILE_TESTS)
  add_subdirectory(tests)
endif()

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_logger
  DEPENDENCIES ${YARP_logger_PUBLIC_DEPS}
  PRIVATE_DEPENDENCIES ${YARP_logger_PRIVATE_DEPS}
)
