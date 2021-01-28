/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
