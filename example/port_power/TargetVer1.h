// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef TARGETVER1_INC
#define TARGETVER1_INC

#include <yarp/os/begin_pack_for_net.h>
class Target {
public:
    NetInt32 x;
    NetInt32 y;
} PACKED_FOR_NET;
#include <yarp/os/end_pack_for_net.h>

#endif
