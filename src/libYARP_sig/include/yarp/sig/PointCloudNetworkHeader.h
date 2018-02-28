/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SIG_POINTCLOUDNETWORKHEADER_H
#define YARP_SIG_POINTCLOUDNETWORKHEADER_H

#include <yarp/conf/system.h>

#include <yarp/os/NetInt32.h>

#include <yarp/sig/PointCloudTypes.h>

namespace yarp {
    namespace sig {
        class PointCloudNetworkHeader;
    }
}


// YARP_BEGIN_PACK
/**
 * @brief The yarp::sig::PointCloudNetworkHeader class
 */
class yarp::sig::PointCloudNetworkHeader
{
public:
    PointCloudNetworkHeader() :    width(10),
                                    height(1),
                                    pointType(0),
                                    isDense(true),
//                                     pose(4, 0.0f),
//                                     orientation(4, 0.0f),
                                    hasCustomData(false),
                                    pad{0,0}
                                    { YARP_UNUSED(pad); }
    // PCL like fields
    yarp::os::NetInt32  width;
    yarp::os::NetInt32  height;
    yarp::os::NetInt32  pointType;       // bitwise of all possible informations -> could also be int64 or just an enum, but I thin bitwise gives more freedom about all possible combinations
    bool                isDense;         // the point cloud is dense if not contains NaN or Inf values
//     yarp::sig::Vector   pose;            // translation from origin -- could be an Eigen::Vector4f for better PCL compatibility if yarp can afford to depend from it
//     yarp::sig::Vector   orientation;     // orientation wrt origin  -- could be an Eigen::Quaternion for better PCL compatibility if yarp can afford to depend from it

    // YARPish fileds
//     char                *data;           // actual pointCloud data.

    // Other
    bool                hasCustomData;   // this guy is mis-aligned --> for future use, to have addictional data not initially forseen or fields from ROS which does not fit into yarp pointTypes
//     yarp::os::Bottle    *customData;     // used in case of custom data, to check if a different type can be better optimized for transmission?
private:
    char pad[2]; // char vector to align the bytes of the headers
};
// YARP_END_PACK



#endif // YARP_SIG_POINTCLOUDNETWORKHEADER_H
