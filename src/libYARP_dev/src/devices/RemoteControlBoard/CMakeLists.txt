# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

set(rmc_core_srcs src/devices/RemoteControlBoard/RemoteControlBoard.cpp)

set(rmc_core_hrds src/devices/RemoteControlBoard/RemoteControlBoard.h)


set(rmc_core_srcs ${rmc_core_srcs}
                  src/devices/RemoteControlBoard/stateExtendedReader.cpp)

set(rmc_core_hrds ${rmc_core_hrds}
                  src/devices/RemoteControlBoard//stateExtendedReader.hpp)


# using PARENT_SCOPE to make files visible from upper level

set(rmc_core_srcs ${rmc_core_srcs} PARENT_SCOPE)
set(rmc_core_hrds ${rmc_core_hrds} PARENT_SCOPE)


set(rmc_core_incl_dirs ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
