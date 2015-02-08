// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_IMAGENETWORKHEADER_
#define _YARP2_IMAGENETWORKHEADER_

#include <yarp/conf/system.h>

#include <yarp/os/NetInt32.h>
#include <yarp/os/Bottle.h>

#include <yarp/sig/Image.h>

namespace yarp {
    namespace sig {
        class ImageNetworkHeader;
    }
}

/**
 *
 * Byte order in image header for network transmission.
 *
 */
YARP_BEGIN_PACK
class yarp::sig::ImageNetworkHeader
{
public:

    yarp::os::NetInt32 listTag;
    yarp::os::NetInt32 listLen;
    yarp::os::NetInt32 paramNameTag;
    yarp::os::NetInt32 paramName;
    yarp::os::NetInt32 paramIdTag;
    yarp::os::NetInt32 id;
    yarp::os::NetInt32 paramListTag;
    yarp::os::NetInt32 paramListLen;
    yarp::os::NetInt32 depth;
    yarp::os::NetInt32 imgSize;
    yarp::os::NetInt32 quantum;
    yarp::os::NetInt32 width;
    yarp::os::NetInt32 height;
    yarp::os::NetInt32 paramBlobTag;
    yarp::os::NetInt32 paramBlobLen;

    void setFromImage(const Image& image) {
        listTag = BOTTLE_TAG_LIST;
        listLen = 4;
        paramNameTag = BOTTLE_TAG_VOCAB;
        paramName = VOCAB3('m','a','t');
        paramIdTag = BOTTLE_TAG_VOCAB;
        id = image.getPixelCode();
        paramListTag = BOTTLE_TAG_LIST + BOTTLE_TAG_INT;
        paramListLen = 5;
        depth = image.getPixelSize();
        imgSize = image.getRawImageSize();
        quantum = image.getQuantum();
        width = image.width();
        height = image.height();
        paramBlobTag = BOTTLE_TAG_BLOB;
        paramBlobLen = image.getRawImageSize();
    }

} PACKED_FOR_NET;
YARP_END_PACK

#endif
