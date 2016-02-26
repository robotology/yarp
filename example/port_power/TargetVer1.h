/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
