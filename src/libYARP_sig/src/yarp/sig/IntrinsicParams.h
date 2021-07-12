/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_INTRINSICPARAMS_H
#define YARP_SIG_INTRINSICPARAMS_H

#include <yarp/os/Log.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Property.h>
#include <yarp/sig/api.h>

namespace yarp {
namespace sig {
/**
 * @brief The YarpDistortion enum to define the
 * type of the distortion model of the camera.
 *
 * In geometric optics, distortion is a deviation from rectilinear projection;
 * a projection in which straight lines in a scene remain straight in an image.
 * Although distortion can be irregular or follow many patterns, the most commonly
 * encountered distortions are radially symmetric. These can be represent by models
 * that allow us to undistort the images knowing the parameters of the distortion
 * (k1, k2, t1, t2, k3).
 */
enum class YarpDistortion : std::int32_t
{
    YARP_DISTORTION_NONE,  /**< Rectilinear images. No distortion compensation required. */
    YARP_PLUMB_BOB,        /**< Plumb bob distortion model */
    YARP_FISH_EYE,         /**< Fish eye distortion model */
    YARP_UNSUPPORTED,      /**< Unsupported distortion model */
    YARP_DISTORTION_COUNT, /**< Number of enumeration values. Not a valid input: intended to be used in for-loops. */

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5
    YARP_PLUM_BOB YARP_DEPRECATED_MSG("Use YARP_PLUMB_BOB instead") = YARP_PLUMB_BOB, /**< Deprecated name for plumb bob distortion model */
#endif
};

/**
 * @brief The IntrinsicParams struct to handle the intrinsic parameter
 * of cameras(RGB and RGBD either).
 */
struct YARP_sig_API IntrinsicParams : public yarp::os::Portable
{
    /**
     * @brief The DistortionModel struct representing the distortion model
     * of the camera.
     */
    struct YARP_sig_API DistortionModel
    {
        double k1;
        double k2;
        double t1;
        double t2;
        double k3;
        YarpDistortion type;

        DistortionModel(): k1(0.0), k2(0.0),
                           t1(0.0), t2(0.0),
                           k3(0.0), type(YarpDistortion::YARP_DISTORTION_NONE) {}
    };

    /**
     * @brief IntrinsicParams, default constructor
     */
    IntrinsicParams();

    /**
     * @brief IntrinsicParams
     * @param intrinsic, Property containing the value for filling the struct.
     * @param isOptional, flag to explicitate if it is optional if this struct
     * is used in parsing.
     * @note It asserts if the Property is malformed. The physical focal length and
     * the distortion model are optional,
     * fields principalPointX principalPointY focalLengthX focalLengthY are required.
     */
    IntrinsicParams(const yarp::os::Property &intrinsic, bool isOptional=false);

    /**
     * @brief toProperty, convert the struct to a Property.
     * @param intrinsic[out], Property generated from the struct.
     */
    void toProperty(yarp::os::Property& intrinsic) const;

    /**
     * @brief fromProperty, fill the struct using the data stored in a Property.
     * @param intrinsic[in], input property.
     * @note It asserts if the Property is malformed. The physical focal length and
     * the distortion model are optional,
     * fields principalPointX principalPointY focalLengthX focalLengthY are required.
     */
    void fromProperty(const yarp::os::Property& intrinsic);

    bool read(yarp::os::ConnectionReader& reader) override;
    bool write(yarp::os::ConnectionWriter& writer) const override;

    double   physFocalLength;        /**< Physical focal length of the lens (m) */
    double   principalPointX;        /**< Horizontal coordinate of the principal point of the image, as a pixel offset from the left edge */
    double   principalPointY;        /**< Vertical coordinate of the principal point of the image, as a pixel offset from the top edge */
    double   focalLengthX;           /**< Result of the product of the physical focal length(mm) and the size sx of the individual imager elements (pixels per mm) */
    double   focalLengthY;           /**< Result of the product of the physical focal length(mm) and the size sy of the individual imager elements (pixels per mm) */
    DistortionModel distortionModel; /**< Distortion model of the image */
    bool     isOptional;
};

} // namespace sig
} // namespace yarp



#endif // YARP_SIG_INTRINSICPARAMS_H
