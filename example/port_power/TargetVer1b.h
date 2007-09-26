// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef TARGETVER1B_INC
#define TARGETVER1B_INC

#include <yarp/os/Bottle.h>

#include <yarp/os/begin_pack_for_net.h>
class Target {
public:
    Target() {
        tag = BOTTLE_TAG_LIST + BOTTLE_TAG_INT;
        len = 2;
    }

    NetInt32 tag;
    NetInt32 len;
    NetInt32 x;
    NetInt32 y;
} PACKED_FOR_NET;
#include <yarp/os/end_pack_for_net.h>

#endif
