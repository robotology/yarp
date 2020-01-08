/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SIG_IMAGENETWORKHEADER_H
#define YARP_SIG_IMAGENETWORKHEADER_H

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

    ImageNetworkHeader() : listTag(0), listLen(0), paramNameTag(0),
                           paramName(0), paramIdTag(0), id(0),
                           paramListTag(0), paramListLen(0), depth(0),
                           imgSize(0), quantum(0), width(0),
                           height(0), paramBlobTag(0), paramBlobLen(0) {}

    void setFromImage(const Image& image) {
        listTag = BOTTLE_TAG_LIST;
        listLen = 4;
        paramNameTag = BOTTLE_TAG_VOCAB;
        paramName = yarp::os::createVocab('m','a','t');
        paramIdTag = BOTTLE_TAG_VOCAB;
        id = image.getPixelCode();
        paramListTag = BOTTLE_TAG_LIST + BOTTLE_TAG_INT32;
        paramListLen = 5;
        depth = image.getPixelSize();
        imgSize = image.getRawImageSize();
        quantum = image.getQuantum();
        width = image.width();
        height = image.height();
        paramBlobTag = BOTTLE_TAG_BLOB;
        paramBlobLen = image.getRawImageSize();
    }

};
YARP_END_PACK

#endif // YARP_SIG_IMAGENETWORKHEADER_H
