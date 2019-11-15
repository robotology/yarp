/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SIG_POINTCLOUDUTILS_H
#define YARP_SIG_POINTCLOUDUTILS_H

#include <yarp/sig/Image.h>
#include <yarp/sig/IntrinsicParams.h>
#include <yarp/sig/PointCloud.h>

namespace yarp {
namespace sig{
/**
 * \ingroup sig_class
 *
 * PointCloud utilities.
 */
namespace utils
{

/**
 * @brief depthToPC, compute the PointCloud given depth image and the intrinsic parameters of the camera.
 * @param[in] depth, the input depth image.
 * @param[in] intrinsic, intrinsic parameter of the camera.
 * @note the intrinsic parameters are the one of the depth sensor if the depth frame IS NOT aligned with the
 * colored one. On the other hand use the intrinsic parameters of the RGB camera if the frames are aligned.
 * @return the pointcloud obtained by the de-projection.
 */
YARP_sig_API yarp::sig::PointCloud<yarp::sig::DataXYZ> depthToPC(const yarp::sig::ImageOf<yarp::sig::PixelFloat>& depth,
                                                                 const yarp::sig::IntrinsicParams& intrinsic);

/**
 * @brief depthRgbToPC, compute the colored PointCloud given depth image, color image and the intrinsic
 * parameters of the camera.
 * @param[in] depth, the input depth image.
 * @param[in] color, the input color image.
 * @param[in] intrinsic, intrinsic parameter of the camera.
 * @note the intrinsic parameters are the one of the depth sensor if the depth frame IS NOT aligned with the
 * colored one. On the other hand use the intrinsic parameters of the RGB camera if the frames are aligned.
 * @return the pointcloud obtained by the de-projection.
 */
template<typename T1, typename T2>
yarp::sig::PointCloud<T1> depthRgbToPC(const yarp::sig::ImageOf<yarp::sig::PixelFloat>& depth,
                                       const yarp::sig::ImageOf<T2>& color,
                                       const yarp::sig::IntrinsicParams& intrinsic);
} // namespace utils
} // namespace sig
} // namespace yarp

#include <yarp/sig/PointCloudUtils-inl.h>

#endif // YARP_SIG_POINTCLOUDUTILS_H
