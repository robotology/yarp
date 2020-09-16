/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SIG_POINTCLOUDUTILS_INL_H
#define YARP_SIG_POINTCLOUDUTILS_INL_H

#include <type_traits>


namespace {

template<typename T1,
         typename T2,
         std::enable_if_t<std::is_same<T1, yarp::sig::DataXYZRGBA>::value &&
                          (std::is_same<T2, yarp::sig::PixelRgb>::value ||
                           std::is_same<T2, yarp::sig::PixelBgr>::value), int> = 0
>
inline void copyColorData(yarp::sig::PointCloud<T1>& pointCloud,
                          const yarp::sig::ImageOf<T2>& color,
                          const size_t u,
                          const size_t v)
{
    pointCloud(u,v).r = color.pixel(u,v).r;
    pointCloud(u,v).g = color.pixel(u,v).g;
    pointCloud(u,v).b = color.pixel(u,v).b;
}

template<typename T1,
         typename T2,
         std::enable_if_t<std::is_same<T1, yarp::sig::DataXYZRGBA>::value &&
                          (std::is_same<T2, yarp::sig::PixelRgba>::value ||
                           std::is_same<T2, yarp::sig::PixelBgra>::value), int> = 0
>
inline void copyColorData(yarp::sig::PointCloud<T1>& pointCloud,
                          const yarp::sig::ImageOf<T2>& color,
                          const size_t u,
                          const size_t v)
{
    pointCloud(u,v).r = color.pixel(u,v).r;
    pointCloud(u,v).g = color.pixel(u,v).g;
    pointCloud(u,v).b = color.pixel(u,v).b;
    pointCloud(u,v).a = color.pixel(u,v).a;
}

template<typename T1,
         typename T2,
         std::enable_if_t<!std::is_same<T1, yarp::sig::DataXYZRGBA>::value ||
                          (!std::is_same<T2, yarp::sig::PixelRgb>::value &&
                           !std::is_same<T2, yarp::sig::PixelBgr>::value &&
                           !std::is_same<T2, yarp::sig::PixelRgba>::value &&
                           !std::is_same<T2, yarp::sig::PixelBgra>::value), int> = 0
>
inline void copyColorData(yarp::sig::PointCloud<T1>& pointCloud,
                          const yarp::sig::ImageOf<T2>& color,
                          size_t u,
                          size_t v)
{
}

} // namespace

template<typename T1, typename T2>
yarp::sig::PointCloud<T1> yarp::sig::utils::depthRgbToPC(const yarp::sig::ImageOf<yarp::sig::PixelFloat>& depth,
                                                         const yarp::sig::ImageOf<T2>& color,
                                                         const yarp::sig::IntrinsicParams& intrinsic)
{
    yAssert(depth.width()  != 0);
    yAssert(depth.height() != 0);
    yAssert(depth.width()  == color.width());
    yAssert(depth.height() == color.height());
    size_t w = depth.width();
    size_t h = depth.height();
    yarp::sig::PointCloud<T1> pointCloud;
    pointCloud.resize(w, h);

    for (size_t u = 0; u < w; ++u) {
        for (size_t v = 0; v < h; ++v) {
            // Depth
            // De-projection equation (pinhole model):
            //                          x = (u - ppx)/ fx * z
            //                          y = (v - ppy)/ fy * z
            //                          z = z
            pointCloud(u,v).x = (u - intrinsic.principalPointX)/intrinsic.focalLengthX*depth.pixel(u,v);
            pointCloud(u,v).y = (v - intrinsic.principalPointY)/intrinsic.focalLengthY*depth.pixel(u,v);
            pointCloud(u,v).z = depth.pixel(u,v);

            copyColorData(pointCloud, color, u, v);
        }
    }
    return pointCloud;
}

#endif // YARP_SIG_POINTCLOUDUTILS_INL_H
