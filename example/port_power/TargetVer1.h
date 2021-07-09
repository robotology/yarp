/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef TARGETVER1_INC
#define TARGETVER1_INC

#include <yarp/conf/system.h>

YARP_BEGIN_PACK
class Target {
public:
    NetInt32 x;
    NetInt32 y;
};
YARP_END_PACK

#endif
