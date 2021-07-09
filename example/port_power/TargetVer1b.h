/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef TARGETVER1B_INC
#define TARGETVER1B_INC

#include <yarp/conf/system.h>
#include <yarp/os/Bottle.h>

YARP_BEGIN_PACK
class Target {
public:
    Target() {
        tag = BOTTLE_TAG_LIST + BOTTLE_TAG_INT32;
        len = 2;
    }

    NetInt32 tag;
    NetInt32 len;
    NetInt32 x;
    NetInt32 y;
};
YARP_END_PACK

#endif
