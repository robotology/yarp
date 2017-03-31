/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo, Andrea Ruzzenenti
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_MATH_TRANSFORM_H
#define YARP_MATH_TRANSFORM_H

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/api.h>
#include <yarp/math/Math.h>
#include <yarp/math/Quaternion.h>

#define VALID_POSITION         1  // 0b00000001
#define VALID_ROTATION         2  // 0b00000010
#define VALID_LIN_VELOCITY     4  // 0b00000100
#define VALID_LIN_ACCELERATION 8  // 0b00001000
#define VALID_ANG_VELOCITY     16 // 0b00010000
#define VALID_ANG_ACCELERATION 32 // 0b00100000
#define VALID_FULL VALID_POSITION | VALID_ROTATION | VALID_LIN_VELOCITY | VALID_LIN_ACCELERATION | VALID_ANG_VELOCITY | VALID_ANG_ACCELERATION

namespace yarp
{
namespace math
{



class YARP_math_API FrameTransform
{
public:
    std::string src_frame_id;
    std::string dst_frame_id;
    double      timestamp;

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

    yarp::math::Quaternion rotation;
    yarp::sig::Vector      linVelocity;
    yarp::sig::Vector      angVelocity;
    yarp::sig::Vector      linAcceleration;
    yarp::sig::Vector      angAcceleration;
    bool                   isValid;

    FrameTransform();
    FrameTransform(const std::string&  parent,
                   const std::string&  child,
                   double              inTX,
                   double              inTY,
                   double              inTZ,
                   double              inRX,
                   double              inRY,
                   double              inRZ,
                   double              inRW);
    ~FrameTransform(){}
    void              transFromVec(double X, double Y, double Z);
    void              rotFromRPY(double R, double P, double Y);
    yarp::sig::Vector getRPYRot() const;
    yarp::sig::Matrix toMatrix() const;
    bool              fromMatrix(const yarp::sig::Matrix& mat);
    std::string       toString();
};

}//namespace dev
}//namespace yarp

#endif

