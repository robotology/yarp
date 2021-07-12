/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
                                                         const yarp::sig::IntrinsicParams& intrinsic,
                                                         const yarp::sig::utils::OrganizationType organizationType)
{
    yAssert(depth.width()  != 0);
    yAssert(depth.height() != 0);
    yAssert(depth.width()  == color.width());
    yAssert(depth.height() == color.height());
    size_t w = depth.width();
    size_t h = depth.height();
    yarp::sig::PointCloud<T1> pointCloud;
    if (organizationType == yarp::sig::utils::OrganizationType::Organized){
        pointCloud.resize(w, h);
    }
    for (size_t u = 0; u < w; ++u) {
        for (size_t v = 0; v < h; ++v) {
            if (organizationType == yarp::sig::utils::OrganizationType::Organized){
                // Depth
                // De-projection equation (pinhole model):
                //                          x = (u - ppx)/ fx * z
                //                          y = (v - ppy)/ fy * z
                //                          z = z
                pointCloud(u,v).x = (u - intrinsic.principalPointX)/intrinsic.focalLengthX*depth.pixel(u,v);
                pointCloud(u,v).y = (v - intrinsic.principalPointY)/intrinsic.focalLengthY*depth.pixel(u,v);
                pointCloud(u,v).z = depth.pixel(u,v);
                copyColorData(pointCloud, color, u, v);

            } else if (organizationType == yarp::sig::utils::OrganizationType::Unorganized) {
                if (depth.pixel(u,v) > 0){
                    T1 point;
                    point.x = (u - intrinsic.principalPointX)/intrinsic.focalLengthX*depth.pixel(u,v);
                    point.y = (v - intrinsic.principalPointY)/intrinsic.focalLengthY*depth.pixel(u,v);
                    point.z = depth.pixel(u,v);
                    point.r = color.pixel(u,v).r;
                    point.g = color.pixel(u,v).g;
                    point.b = color.pixel(u,v).b;
                    pointCloud.push_back(point);
                }
            }
        }
    }
    return pointCloud;
}

#endif // YARP_SIG_POINTCLOUDUTILS_INL_H
