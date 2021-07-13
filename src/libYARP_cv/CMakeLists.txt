# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_cv)

# OpenCV is required, otherwise the OpenCV_INCLUDE_DIR and OpenCV_LIBRARIES
# (used in the CMakeLists.txt file in the src direcrory) are not expanded).
# FIXME Remove this check when OpenCV targets will be used.
if (NOT YARP_HAS_OpenCV)
  return()
endif()

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_cv
  DEPENDENCIES ${YARP_cv_PUBLIC_DEPS}
  PRIVATE_DEPENDENCIES ${YARP_cv_PRIVATE_DEPS}
)
