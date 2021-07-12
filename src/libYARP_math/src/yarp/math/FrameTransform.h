/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MATH_TRANSFORM_H
#define YARP_MATH_TRANSFORM_H

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/api.h>
#include <yarp/math/Quaternion.h>

namespace yarp {
namespace math {

class YARP_math_API FrameTransform : public yarp::os::Portable
{
public:
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) src_frame_id;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) dst_frame_id;
    double timestamp = 0;
    bool   isStatic  = false;

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

    enum display_transform_mode_t
    {
       rotation_as_quaternion=0,
       rotation_as_matrix=1,
       rotation_as_rpy=2
    };

    std::string toString(display_transform_mode_t format= rotation_as_quaternion) const;

    ///////// Serialization methods
    /*
    * Read data from a connection.
    * return true iff data was read correctly
    */
    bool read(yarp::os::ConnectionReader& connection) override;

    /**
    * Write data to a connection.
    * return true iff data was written correctly
    */
    bool write(yarp::os::ConnectionWriter& connection) const override;

    yarp::os::Type getType() const override
    {
        return yarp::os::Type::byName("yarp/frameTransform");
    }
};

} // namespace sig
} // namespace yarp

#endif // YARP_MATH_TRANSFORM_H
