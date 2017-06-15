# Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
# Author: Alberto Cardellino <alberto.cardellino@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


# use PARENT_SCOPE to make the following variables available at the 'parent' cmake

set(RGBDClient_srcs     src/devices/RGBDSensorClient/RGBDSensorClient.cpp
                        src/devices/RGBDSensorClient/RGBDSensorClient_StreamingMsgParser.cpp
                        PARENT_SCOPE)

set(RGBDClient_hrds     src/devices/RGBDSensorClient/RGBDSensorClient.h
                        src/devices/RGBDSensorClient/RGBDSensorClient_StreamingMsgParser.h
                        PARENT_SCOPE)


set(RGBDClient_incl_dirs ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
