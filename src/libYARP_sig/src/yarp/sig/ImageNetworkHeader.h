/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_IMAGENETWORKHEADER_H
#define YARP_SIG_IMAGENETWORKHEADER_H

#include <yarp/conf/system.h>

#include <yarp/os/NetInt32.h>
#include <yarp/os/NetInt16.h>
#include <yarp/os/Bottle.h>

#include <yarp/sig/Image.h>

namespace yarp {
namespace sig {

/**
 * Byte order in image header for network transmission.
 */
YARP_BEGIN_PACK
class ImageNetworkHeader
{
public:
    const yarp::os::NetInt32 listTag{BOTTLE_TAG_LIST};
    const yarp::os::NetInt32 listLen{4};
    const yarp::os::NetInt32 paramNameTag{BOTTLE_TAG_VOCAB32};
    const yarp::os::NetInt32 paramName{yarp::os::createVocab32('m','a','t')};
    const yarp::os::NetInt32 paramIdTag{BOTTLE_TAG_VOCAB32};
    yarp::os::NetInt32 id{0};
    const yarp::os::NetInt32 paramListTag{BOTTLE_TAG_LIST + BOTTLE_TAG_INT32};
    // WARNING This is 5 and not 6 because quantum and topIsLow are
    //         transmitted in the same 32 bits for compatibility with
    //         YARP 3.4 and older
    const yarp::os::NetInt32 paramListLen{5};
    yarp::os::NetInt32 depth{0};
    yarp::os::NetInt32 imgSize{0};
    yarp::os::NetInt16 quantum{0};
    // WARNING The topIsLowIndex field in the ImageNetworkHeader is `0` for
    //         `true` and `1` for `false` for compatibility with YARP 3.4
    //         and older
    yarp::os::NetInt16 topIsLow{0};
    yarp::os::NetInt32 width{0};
    yarp::os::NetInt32 height{0};
    const yarp::os::NetInt32 paramBlobTag{BOTTLE_TAG_BLOB};
    yarp::os::NetInt32 paramBlobLen{0};

    void setFromImage(const Image& image)
    {
        id = image.getPixelCode();
        depth = image.getPixelSize();
        imgSize = image.getRawImageSize();
        quantum = static_cast<yarp::os::NetInt16>(image.getQuantum());
        topIsLow = image.topIsLowIndex() ? 0 : 1;
        width = image.width();
        height = image.height();
        paramBlobLen = image.getRawImageSize();
    }
};
YARP_END_PACK

} // namespace sig
} // namespace yarp

#endif // YARP_SIG_IMAGENETWORKHEADER_H
