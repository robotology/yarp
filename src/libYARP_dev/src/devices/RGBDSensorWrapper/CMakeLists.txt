# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

# use PARENT_SCOPE to make the following variables available at the 'parent' cmake

set(RGBDWrap_srcs      src/devices/RGBDSensorWrapper/RGBDSensorWrapper.cpp
                        PARENT_SCOPE)

set(RGBDWrap_hrds      src/devices/RGBDSensorWrapper/RGBDSensorWrapper.h
                        PARENT_SCOPE)


set(RGBDWrap_incl_dirs ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
