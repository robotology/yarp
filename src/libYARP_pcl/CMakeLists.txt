# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_pcl)

# PCL is required, otherwise the PCL_INCLUDE_DIRS and PCL_LIBRARIES
# (used in the CMakeLists.txt file in the src direcrory) are not expanded).
# FIXME Remove this check when PCL targets will be used.
if (NOT YARP_HAS_PCL)
  return()
endif()

add_subdirectory(src)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_pcl
                                 DEPENDENCIES ${YARP_pcl_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_pcl_PRIVATE_DEPS})
