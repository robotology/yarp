/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef BLOBHEADER_INC
#define BLOBHEADER_INC

#include <yarp/conf/system.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/NetInt32.h>

namespace yarp {
namespace wire_rep_utils {

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

} // namespace wire_rep_utils
} // namespace yarp

#endif
