/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef BLOBHEADER_INC
#define BLOBHEADER_INC

#include <yarp/conf/system.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/NetInt32.h>

// translate to blobs for now; better translation requires type system
YARP_BEGIN_PACK
class BlobNetworkHeader {
public:
    yarp::os::NetInt32 listTag;
    yarp::os::NetInt32 listLen;
    yarp::os::NetInt32 blobLen;

    void init(int len) {
        // state that the following data is a list containing 1 blob
        listTag = BOTTLE_TAG_LIST + BOTTLE_TAG_BLOB;
        listLen = 1;
        blobLen = len;
    }

};
YARP_END_PACK

#endif
