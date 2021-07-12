/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/PointCloudUtils.h>
#include <algorithm>
#include <cstring>

using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(POINTCLOUDUTILS, "yarp.sig.PointCloudUtils")
}

PointCloud<DataXYZ> utils::depthToPC(const yarp::sig::ImageOf<PixelFloat> &depth,
                                     const yarp::sig::IntrinsicParams &intrinsic)
{
    yCAssert(POINTCLOUDUTILS, depth.width()  != 0);
    yCAssert(POINTCLOUDUTILS, depth.height() != 0);
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
    yCAssert(POINTCLOUDUTILS, depth.width() != 0);
    yCAssert(POINTCLOUDUTILS, depth.height() != 0);

    size_t max_x = roi.max_x == 0 ? depth.width()  : std::min(roi.max_x, depth.width());
    size_t max_y = roi.max_y == 0 ? depth.height() : std::min(roi.max_y, depth.height());
    size_t min_x = std::min(roi.min_x, max_x);
    size_t min_y = std::min(roi.min_y, max_y);
    // avoid step larger than ROI and division by zero
    step_x = std::max<size_t>(std::min(step_x, max_x - min_x), 1);
    step_y = std::max<size_t>(std::min(step_y, max_y - min_y), 1);

    PointCloud<DataXYZ> pointCloud;
    size_t size_x = (max_x - min_x) / step_x;
    size_t size_y = (max_y - min_y) / step_y;
    pointCloud.resize(size_x, size_y);

    for (size_t i = 0, u = min_x; u < max_x; i++, u += step_x) {
        for (size_t j = 0, v = min_y; v < max_y; j++, v += step_y) {
            // De-projection equation (pinhole model):
            //                          x = (u - ppx)/ fx * z
            //                          y = (v - ppy)/ fy * z
            //                          z = z
            pointCloud(i, j).x = (u - intrinsic.principalPointX) / intrinsic.focalLengthX * depth.pixel(u, v);
            pointCloud(i, j).y = (v - intrinsic.principalPointY) / intrinsic.focalLengthY * depth.pixel(u, v);
            pointCloud(i, j).z = depth.pixel(u, v);
        }
    }
    return pointCloud;
}
