/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
