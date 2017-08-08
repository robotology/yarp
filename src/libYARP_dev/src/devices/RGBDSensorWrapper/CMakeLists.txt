# Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
# Author: Alberto Cardellino <alberto.cardellino@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# use PARENT_SCOPE to make the following variables available at the 'parent' cmake

set(RGBDWrap_srcs      src/devices/RGBDSensorWrapper/RGBDSensorWrapper.cpp
                        PARENT_SCOPE)

set(RGBDWrap_hrds      src/devices/RGBDSensorWrapper/RGBDSensorWrapper.h
                        PARENT_SCOPE)


set(RGBDWrap_incl_dirs ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
