/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
 * @brief The IntrinsicParams struct to handle the intrinsic parameter
 * of cameras(RGB and RGBD either).
 */
struct YARP_sig_API IntrinsicParams : public yarp::os::Portable
{
    /**
     * @brief The plum_bob struct representing the distortion model
     * of the camera.
     */
    struct YARP_sig_API plum_bob
    {
        double k1;
        double k2;
        double t1;
        double t2;
        double k3;
        plum_bob(): k1(0.0), k2(0.0),
                    t1(0.0), t2(0.0),
                    k3(0.0) {}
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
     * @note It asserts if the Property is malformed. The distortion model is optional,
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
     * @note It asserts if the Property is malformed. The distortion model is optional,
     * fields principalPointX principalPointY focalLengthX focalLengthY are required.
     */
    void fromProperty(const yarp::os::Property& intrinsic);

    bool read(yarp::os::ConnectionReader& reader) override;
    bool write(yarp::os::ConnectionWriter& writer) const override;

    double   principalPointX;
    double   principalPointY;
    double   focalLengthX;
    double   focalLengthY;
    plum_bob distortionModel;
    bool     isOptional;
};

} // namespace sig
} // namespace yarp



#endif // YARP_SIG_INTRINSICPARAMS_H
