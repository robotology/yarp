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

PointCloud<DataXYZ> utils::depthToPC(const yarp::sig::ImageOf<PixelFloat>& depth,
                                     const yarp::sig::IntrinsicParams& intrinsic,
                                     const PCL_ROI& roi,
                                     size_t step_x,
                                     size_t step_y)
{
    yAssert(depth.width() != 0);
    yAssert(depth.height() != 0);

    size_t max_x = 0;
    size_t max_y = 0;
    if (roi.max_x == 0) max_x = depth.width();
    if (roi.max_y == 0) max_y = depth.height();
    max_x = max_x < depth.width()  ? max_x : depth.width();
    max_y = max_y < depth.height() ? max_y : depth.height();

    PointCloud<DataXYZ> pointCloud;
    pointCloud.resize(max_x-roi.min_x, max_y-roi.min_y);

    for (    size_t u = roi.min_x; u < max_x; u += step_x) {
        for (size_t v = roi.min_y; v < max_y; v += step_y) {
            // De-projection equation (pinhole model):
            //                          x = (u - ppx)/ fx * z
            //                          y = (v - ppy)/ fy * z
            //                          z = z
            pointCloud(u, v).x = (u - intrinsic.principalPointX) / intrinsic.focalLengthX * depth.pixel(u, v);
            pointCloud(u, v).y = (v - intrinsic.principalPointY) / intrinsic.focalLengthY * depth.pixel(u, v);
            pointCloud(u, v).z = depth.pixel(u, v);
        }
    }
    return pointCloud;
}