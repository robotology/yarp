# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_pcl)

# PCL is required, otherwise the PCL_<component>_INCLUDE_DIR and
# PCL_<component>_LIBRARIES
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
