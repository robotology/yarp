/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MATH_TRANSFORM_H
#define YARP_MATH_TRANSFORM_H

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/api.h>
#include <yarp/math/Math.h>
#include <yarp/math/Quaternion.h>

namespace yarp {
namespace math {

class YARP_math_API FrameTransform
{
public:
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) src_frame_id;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) dst_frame_id;
    double timestamp;

    struct Translation_t
    {
        double tX;
        double tY;
        double tZ;

        void set(double x, double y, double z)
        {
            tX = x;
            tY = y;
            tZ = z;
        }
    } translation;

    Quaternion rotation;

    FrameTransform();

    FrameTransform(const std::string& parent,
                   const std::string& child,
                   double             inTX,
                   double             inTY,
                   double             inTZ,
                   double             inRX,
                   double             inRY,
                   double             inRZ,
                   double             inRW);

    ~FrameTransform();

    void transFromVec(double X, double Y, double Z);
    void rotFromRPY(double R, double P, double Y);
    yarp::sig::Vector getRPYRot() const;

    yarp::sig::Matrix toMatrix() const;
    bool fromMatrix(const yarp::sig::Matrix& mat);

    std::string toString() const;
};

} // namespace sig
} // namespace yarp

#endif // YARP_MATH_TRANSFORM_H
