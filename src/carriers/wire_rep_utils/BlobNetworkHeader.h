// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef BLOBHEADER_INC
#define BLOBHEADER_INC

#include <yarp/os/Bottle.h>
#include <yarp/os/NetInt32.h>

// translate to blobs for now; better translation requires type system
#include <yarp/os/begin_pack_for_net.h>
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

} PACKED_FOR_NET;
#include <yarp/os/end_pack_for_net.h>

#endif
