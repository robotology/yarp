# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_rosmsg)

add_subdirectory(src)


# Install all CMake files
include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_rosmsg_native
                                 DEPENDENCIES ${YARP_rosmsg_native_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_rosmsg_native_PRIVATE_DEPS})
foreach(_pkg ${ROS_MSG_PACKAGES})
  yarp_install_basic_package_files(YARP_rosmsg_${_pkg}
                                   DEPENDENCIES ${YARP_rosmsg_${_pkg}_PUBLIC_DEPS}
                                   PRIVATE_DEPENDENCIES ${YARP_rosmsg_${_pkg}_PRIVATE_DEPS})
endforeach()
yarp_install_basic_package_files(YARP_rosmsg
                                 DEPENDENCIES ${YARP_rosmsg_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_rosmsg_PRIVATE_DEPS})
