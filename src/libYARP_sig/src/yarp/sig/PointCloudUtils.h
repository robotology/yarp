/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_POINTCLOUDUTILS_H
#define YARP_SIG_POINTCLOUDUTILS_H

#include <yarp/sig/Image.h>
#include <yarp/sig/IntrinsicParams.h>
#include <yarp/sig/PointCloud.h>

namespace yarp::sig::utils {

enum class OrganizationType{
    Organized,
    Unorganized
};

struct PCL_ROI
{
    size_t min_x {0};
    size_t max_x {0};
    size_t min_y {0};
    size_t max_y {0};
};

/**
 * @brief depthToPC, compute the PointCloud given depth image and the intrinsic parameters of the camera.
 * @param[in] depth, the input depth image.
 * @param[in] intrinsic, intrinsic parameter of the camera.
 * @note the intrinsic parameters are the one of the depth sensor if the depth frame IS NOT aligned with the
 * colored one. On the other hand use the intrinsic parameters of the RGB camera if the frames are aligned.
 * @return the point cloud obtained by the de-projection.
 */
YARP_sig_API yarp::sig::PointCloud<yarp::sig::DataXYZ> depthToPC(const yarp::sig::ImageOf<yarp::sig::PixelFloat>& depth,
                                                                 const yarp::sig::IntrinsicParams& intrinsic);

/**
 * @brief depthToPC, compute the PointCloud given depth image, the intrinsic parameters of the camera and a Region Of Interest.
 * @param[in] depth, the input depth image.
 * @param[in] intrinsic, intrinsic parameter of the camera.
 * @param[in] roi, the Region Of Interest intrinsic of the depth image that we want to convert.
 * @param[in] step_x, the depth image size can be decimated, by selecting a column every step_x;
 * @param[in] step_y, the depth image size can be decimated, by selecting a row every step_y;
 * @param[in] output_order, the string will optionally rearrange data in any combination of positive and negative axes. e.g. "+Z-Y+X".
 * Useful when dealing with not right-handed coordinate systems. Optimized for efficiency during point cloud creation with no additional computation.
 * @note the intrinsic parameters are the one of the depth sensor if the depth frame IS NOT aligned with the
 * colored one. On the other hand use the intrinsic parameters of the RGB camera if the frames are aligned.
 * @return the point cloud obtained by the de-projection.
 */
YARP_sig_API yarp::sig::PointCloud<yarp::sig::DataXYZ> depthToPC(const yarp::sig::ImageOf<yarp::sig::PixelFloat>& depth,
                                                                 const yarp::sig::IntrinsicParams& intrinsic,
                                                                 const yarp::sig::utils::PCL_ROI& roi,
                                                                 size_t step_x=1,
                                                                 size_t step_y=1,
                                                                 const std::string& output_order = "+X+Y+Z");

/**
 * @brief depthRgbToPC, compute the colored PointCloud given depth image, color image and the intrinsic
 * parameters of the camera.
 * @param[in] depth, the input depth image.
 * @param[in] color, the input color image.
 * @param[in] intrinsic, intrinsic parameter of the camera.
 * @param[in] organizationType, if organized the point cloud has size (width, depth). if unorganized the point cloud
 * has size (width*height, 1). Note that in this case the data are organized column-wise (i.e. y,x and not x,y)
 * @param[in] step_x, the depth image size can be decimated, by selecting a column every step_x;
 * @param[in] step_y, the depth image size can be decimated, by selecting a row every step_y;
 * @param[in] output_order, the string will optionally rearrange data in any combination of positive and negative axes. e.g. "+Z-Y+X".
 * Useful when dealing with not right-handed coordinate systems. Optimized for efficiency during point cloud creation with no additional computation.
  * @note the intrinsic parameters are the one of the depth sensor if the depth frame IS NOT aligned with the
 * colored one. On the other hand use the intrinsic parameters of the RGB camera if the frames are aligned.
 * @return the point cloud obtained by the de-projection.
 */
template<typename T1, typename T2>
yarp::sig::PointCloud<T1> depthRgbToPC(const yarp::sig::ImageOf<yarp::sig::PixelFloat>& depth,
                                       const yarp::sig::ImageOf<T2>& color,
                                       const yarp::sig::IntrinsicParams& intrinsic,
                                       const yarp::sig::utils::OrganizationType organizationType = yarp::sig::utils::OrganizationType::Organized,
                                       size_t step_x=1,
                                       size_t step_y=1,
                                       const std::string& output_order = "+X+Y+Z");
} // namespace yarp::sig::utils

#include <yarp/sig/PointCloudUtils-inl.h>

#endif // YARP_SIG_POINTCLOUDUTILS_H
