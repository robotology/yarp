/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SIG_POINTCLOUDNETWORKHEADER_H
#define YARP_SIG_POINTCLOUDNETWORKHEADER_H

#include <yarp/sig/api.h>

#include <yarp/os/NetInt32.h>

#include <yarp/sig/PointCloudTypes.h>

namespace yarp {
namespace sig {

YARP_BEGIN_PACK
/**
 * @brief The yarp::sig::PointCloudNetworkHeader class
 */
class YARP_sig_API PointCloudNetworkHeader
{
public:
    PointCloudNetworkHeader() : width(0),
                                height(0),
                                pointType(0),
                                isDense(1)
    {}

    // PCL like fields
    yarp::os::NetInt32  width;
    yarp::os::NetInt32  height;
    yarp::os::NetInt32  pointType;       // bitwise of all possible information -> could also be int64 or just an enum, but I thin bitwise gives more freedom about all possible combinations
    std::int8_t         isDense;         // the point cloud is dense if not contains NaN or Inf values

};
YARP_END_PACK

} // namespace sig
} // namespace yarp


#endif // YARP_SIG_POINTCLOUDNETWORKHEADER_H
