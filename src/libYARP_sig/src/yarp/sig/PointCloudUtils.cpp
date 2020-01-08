/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/sig/PointCloudUtils.h>
#include <cstring>

using namespace yarp::sig;

PointCloud<DataXYZ> utils::depthToPC(const yarp::sig::ImageOf<PixelFloat> &depth,
                                     const yarp::sig::IntrinsicParams &intrinsic)
{
    yAssert(depth.width()  != 0);
    yAssert(depth.height() != 0);
    size_t w = depth.width();
    size_t h = depth.height();
    PointCloud<DataXYZ> pointCloud;
    pointCloud.resize(w, h);

    for (size_t u = 0; u < w; ++u) {
        for (size_t v = 0; v < h; ++v) {
            // De-projection equation (pinhole model):
            //                          x = (u - ppx)/ fx * z
            //                          y = (v - ppy)/ fy * z
            //                          z = z
            pointCloud(u,v).x = (u - intrinsic.principalPointX)/intrinsic.focalLengthX*depth.pixel(u,v);
            pointCloud(u,v).y = (v - intrinsic.principalPointY)/intrinsic.focalLengthY*depth.pixel(u,v);
            pointCloud(u,v).z = depth.pixel(u,v);
        }
    }
    return pointCloud;
}
