# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

add_subdirectory(thrift)
add_subdirectory(rosmsg)

if(NOT CREATE_SHARED_LIBS)
  include(YarpInstallBasicPackageFiles)
  yarp_install_basic_package_files(YARP_idl_tools
                                   INCLUDE_CONTENT "include(\"\\\${YARP_MODULE_DIR}/YarpIDL.cmake\")")
endif()
